/*
 * HALclkInitASM.S
 * This file contains the implementation for setting up PLL, Q6 clock of LPASS
 *                   Edit History

   $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/adsp/hw/sdm670/HALclkInitASM.S#9 $

     #when       who                  what, where, why
    --------   -------------       ----------------------------------------------
    06/13/2017   vasavit              Created file based on sdm845.
 *
 */
#include "HALclkHWIOASM.h"


/////////////////////////////////////////////////////////////////////////////////
//
//       Additional definitions and bit masks not defined in HWIO inc files.
// 
/////////////////////////////////////////////////////////////////////////////////


#define HALCLK_WAIT_5_US                      0x60
#define HALCLK_WAIT_10_US                     0xC0

#define SSC_QDSP6SS_PLL_CONFIG_CTL_VAL        0x20485699
#define SSC_QDSP6SS_PLL_CONFIG_CTL_U_VAL      0x00002067
#define SSC_SCC_PLL_CONFIG_CTL_VAL            0x20485699
#define SSC_SCC_PLL_CONFIG_CTL_U_VAL          0x00002067

#define SSC_QDSP6SS_PLL_USER_CTL_VAL          0x00000007
#define SSC_QDSP6SS_PLL_USER_CTL_U_VAL        0x00004804
#define SSC_SCC_PLL_USER_CTL_VAL              0x00000107
#define SSC_SCC_PLL_USER_CTL_U_VAL            0x00004804

#define SSC_QDSP6SS_PLL_TEST_CTL_VAL          0x40000000
#define SSC_QDSP6SS_PLL_TEST_CTL_U_VAL        0x00000000
#define SSC_SCC_PLL_TEST_CTL_VAL              0x40000000
#define SSC_SCC_PLL_TEST_CTL_U_VAL            0x00000000

// Q6SS Core clock , AHB clock configuration values.
#define SSC_MAIN_BUS_CFG_RCG_DIV_VAL          0x5
#define SSC_MAIN_BUS_CFG_RCG_SRC_SEL_VAL      0x6 
#define SSC_PLL_L_VAL                         0x1F
#define SSC_PLL_FRAC_VAL                      0x4000
#define SSC_PLL_CAL_L_VAL                     0x1F

#define SSC_QDSP6SS_STRAP_ACC_VAL             0x0
#define SSC_QDSP6SS_RCG_SRC_SEL_VAL           0x2
#define SSC_QDSP6SS_RCG_DIV_VAL               0x1
#define SSC_QDSP6SS_PLL_L_VAL                 0x31
#define SSC_QDSP6SS_PLL_FRAC_VAL              0x0
#define SSC_QDSP6SS_PLL_CAL_L_VAL             0x35

#define SSC_SCC_Q6_MISC_VAL                   0x1

#define SSC_PLL_OPMODE_RUN                    0x1

//*******************************************************************
// SSC Dividers based on Main Bus RCG.
//*******************************************************************

#define SCC_CDIV_CCD_DIV_CDIVR                                 0x1
#define SCC_CDIV_SDC_DIV_CDIVR                                 0x1
#define SCC_QTIMER_TMR_CDIV_DIV_CDIVR                          0x4
#define SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR                        0x1
#define SCC_CDIV_QUPV3_CORE_DIV_CDIVR                          0x1
#define SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR                        0x1
#define SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR                        0x1
#define SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR                    0x1


//*******************************************************************
// debug markers for SMMU fault issue
//*******************************************************************

#define SYSDRV_BSP_INIT_ENTER    0x40
#define SYSDRV_BUSYWAIT_INIT_ENTER  0x41
#define SYSDRV_BUSYWAIT_INIT_EXIT  0x42
#define SYSDRV_CFG_CTL_EXIT        0x43
#define SYSDRV_USER_CTL_EXIT       0x44
#define SYSDRV_BUSYWAIT_2_ENTER       0x45
#define SYSDRV_TEST_CTL_EXIT           0x46
#define SYSDRV_SCC_PLL_PROG_INTERMEDIATE  0x47
#define SYSDRV_SCC_PLL_PROG_EXIT   0x48
#define SYSDRV_BUSDOMAIN_CONFIGURE_EXIT 0x49
#define SYSDRV_CDIV_CONFIGURE_EXIT 0x4A
#define SYSDRV_SCC_MAIN_CONFIGURE_EXIT 0x4B
#define SYSDRV_AHB_QTMR_ENABLE_EXIT 0x4C
#define SYSDRV_SMEMCLK_ENABLE_EXIT 0x4D
#define SYSDRV_Q6_MISC_PROG_EXIT 0x4E
#define SYSDRV_SSC_Q6PLL_L_VAL_PROG_EXIT 0x4F
#define SYSDRV_SSC_Q6PLL_CAL_L_VAL_PROG_EXIT 0x50
#define SYSDRV_SSC_Q6PLL_FRAC_VAL_PROG_EXIT 0x51
#define SYSDRV_SSC_Q6PLL_OUTPUTS_TURNOFF_EXIT 0x52
#define SYSDRV_SSC_Q6PLL_PLL_STANDBY_EXIT 0x53
#define SYSDRV_QDSP6PLL_ENABLE_INTERMEDIATE 0x54
#define SYSDRV_QDSP6PLL_RUN_MODE_EXIT 0x55
#define SYSDRV_QDSP6PLL_LOCK_DET_EXIT 0x56
#define SYSDRV_QDSP6SS_PLL_BRINGUP_EXIT 0x57
#define SYSDRV_QDSP6SS_CORE_CLK_CONFIG_EXIT 0x58
#define SYSDRV_Q6_CORE_DOMAIN_EXIT 0x59
#define SYSDRV_BUSYWAIT_DEINIT_EXIT 0x5A
#define SYSDRV_Q6_AHBS_CLK_DISABLED 0x5B
#define SYSDRV_QTIMER_TMR_CLK_ENABLE 0x5C
#define SYSDRV_Q6_AHBM_CLK_DISABLED 0x5D
#define SYSDRV_Q6_AHBS_CLK_ENABLED 0x5E
#define SYSDRV_Q6_AHBS_CLK_OFF_CHECK_COMPLETE 0x5F


/////////////////////////////////////////////////////////////////////////////////
//
//                 Macros Start Here.
// 
/////////////////////////////////////////////////////////////////////////////////


    //
    // MACRO:  WRITE_REGISTER
    //
    // DESCRIPTION:
    //   This macro writes an unmasked value to a register.  Any contents will
    //   be overwritten in the proccess.  r6 and r7 may be clobbered.
    //

    .macro write_register  val
        {
            r6 = \val
            r7 = r0
        }
        memw(r0) = r6
        barrier
    .endm


    //
    // MACRO:  CLEAR_BITS
    //
    // DESCRIPTION:
    //   This macro clears one ore more bits in a register.  r4, r5, r6, and r7
    //   may be clobbered when running this macro.
    //

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


    //
    // MACRO:  SET_BITS
    //
    // DESCRIPTION:
    //   This macro sets one or more bits in a register.  r4, r5 and r7 will be 
    //   clobbered when using this macro.
    //

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

    .macro __busywait_init
        r0 = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(0)
        memw(r0) = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK
        r0 = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(1)
        memw(r0) = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK
        r0 = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(2)
        memw(r0) = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK
        barrier
    .endm

    .macro __busywait_deinit
        r0 = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(0)
        memw(r0) = ##0x0
        r0 = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(1)
        memw(r0) = ##0x0
        r0 = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(2)
        memw(r0) = ##0x0
        barrier
    .endm

     // Expects the debug marker to be updated in r0, r19 has the address from ctr0.S
    .macro __update_dbg_marker
      memw(r19) = r0
      dccleaninva(r19)
	  
    .endm
/////////////////////////////////////////////////////////////////////////////////
//
//         Executable code starts here.
// 
/////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------
// FUNCTION: __HEXAGON_BSP_INIT
//
// DESCRIPTION:
//   This is the main API for this file.  This function will check the status of the
//   main LPASS AUDIO PLL  , LPASS DIGITAL PLL and  SCCPLL , SCCQDSP6SSPLL configure it if it is not running.  Otherwise, it will just
//   add a HW vote on behalf of the LPASS processor.
//
//   Enable QTimer and AHB clocks and configure them to max frequency at NOM Voltage.Configure SCC PLL as part of configuring these clocks.
//   
//   The QDSP6 clock will then be initialized if not already set up.  The 
//   purpose of this is to ensure the QDSP6 clock is running at maximum
//   frequency under NOM voltage.
//
//
// ASSUMPTIONS:
//   There is no stack available when this is called, so only device registers can be
//   accessed.
//----------------------------------------------------------------------------------
#if defined(SIM)
  // No initialization.
#else
   .section .start,"ax",@progbits    
   .falign
   .global __hexagon_bsp_init
   .type  __hexagon_bsp_init, @function
   
   .global BSP_INIT_SYSTEM_STATE
    BSP_INIT_SYSTEM_STATE:     .word 0
   

__hexagon_bsp_init:

    r19 = add(r25 , ##BSP_INIT_SYSTEM_STATE)

    r0 = ##SYSDRV_BSP_INIT_ENTER
  __update_dbg_marker

  r0 = ##HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR
  r1 = ##HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKEN_BMSK
  memw(r0) = r1
  barrier
  
  r0 = ##SYSDRV_BUSYWAIT_INIT_ENTER
  __update_dbg_marker
  
   __busywait_init

   r0 = ##SYSDRV_BUSYWAIT_INIT_EXIT
  __update_dbg_marker
  
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
    if (!p0.new) jump:t __buswait_loop
  }
  {
    r1 = r8
    r2 = r9
    r3 = r10
  }
  jumpr r13
  
clk_enable_check:

  r1 = memw(r0)
  r2 = ##HWIO_SSC_SCC_SMEM_CBCR_CLK_OFF_BMSK
  r3 = and(r1,r2)
clk_off:
  {
   p0 = cmp.eq(r2,r3)
   r1 = memw(r0)
   r3 = and(r1,r2)
   if(p0.new) jump:t clk_off		   
  }
  jumpr r13  
  
start_prog:

  //****************************************************************************
  //
  //                    SSC and LPASS PLL INITIAL SETUP
  //
  //****************************************************************************

v1_pll_configs:
   r14 = ##SSC_PLL_L_VAL
   r15 = ##SSC_PLL_FRAC_VAL
   r16 = ##SSC_PLL_CAL_L_VAL
   r17 = ##SSC_SCC_Q6_MISC_VAL
   r18 = ##SSC_QDSP6SS_PLL_L_VAL
   r21 = ##SSC_QDSP6SS_PLL_FRAC_VAL
   r20 = ##SSC_QDSP6SS_PLL_CAL_L_VAL
    
   jump program_pll
   
program_pll:

   
  //*******************************************************************
  // Program the PLL_CONFIG_CTL values based on recommendations.
  //*******************************************************************


   r0 = ##HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR
   memw(r0) = ##SSC_QDSP6SS_PLL_CONFIG_CTL_VAL

   r0 = ##HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR
   memw(r0) = ##SSC_QDSP6SS_PLL_CONFIG_CTL_U_VAL
   
   r0 = ##HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR
   memw(r0) = ##SSC_SCC_PLL_CONFIG_CTL_VAL

   r0 = ##HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR
   memw(r0) = ##SSC_SCC_PLL_CONFIG_CTL_U_VAL
   
   r0 = ##SYSDRV_CFG_CTL_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  // Program the USER_CTL and USER_CTL_U values based on recommendations.
  //*******************************************************************


   r0 = ##HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR
   memw(r0) = ##SSC_QDSP6SS_PLL_USER_CTL_VAL

   r0 = ##HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR
   memw(r0) = ##SSC_QDSP6SS_PLL_USER_CTL_U_VAL
   
   r0 = ##HWIO_SSC_SCC_PLL_USER_CTL_ADDR
   memw(r0) = ##SSC_SCC_PLL_USER_CTL_VAL

   r0 = ##HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR
   memw(r0) = ##SSC_SCC_PLL_USER_CTL_U_VAL

   barrier
  
   r0 = ##SYSDRV_USER_CTL_EXIT
  __update_dbg_marker

  //*******************************************************************
  // Program the TEST_CTL and TEST_CTL_U values based on recommendations.
  //*******************************************************************

   r0 = ##HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR
   memw(r0) = ##SSC_QDSP6SS_PLL_TEST_CTL_VAL

   r0 = ##HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR
   memw(r0) = ##SSC_QDSP6SS_PLL_TEST_CTL_U_VAL
   
   r0 = ##HWIO_SSC_SCC_PLL_TEST_CTL_ADDR
   memw(r0) = ##SSC_SCC_PLL_TEST_CTL_VAL

   r0 = ##HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR
   memw(r0) = ##SSC_SCC_PLL_TEST_CTL_U_VAL

   barrier

    r0 = ##SYSDRV_TEST_CTL_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  //
  // Begin main PLL programming sequence here.
  //
  //*******************************************************************

  //*******************************************************************
  // Set the desired L-value.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_L_VAL_ADDR
  r1 = r14
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Duplicate the L-value to the CALIB_L register.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR
  r1 = r16
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Program the fractional divider value.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR
  r1 = r15
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
  
  r0 = ##SYSDRV_SCC_PLL_PROG_INTERMEDIATE
  __update_dbg_marker
  
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

    r0 = ##SYSDRV_SCC_PLL_PROG_EXIT
  __update_dbg_marker
  
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
  clear_bits HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_SEL_BMSK
  clear_bits HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_DIV_BMSK
  
  r1 = memw(r0)

  {
    r2 = ##(SSC_MAIN_BUS_CFG_RCG_SRC_SEL_VAL << HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_SEL_SHFT)
    r3 = ##(SSC_MAIN_BUS_CFG_RCG_DIV_VAL << HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_DIV_SHFT)
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  
  barrier

  r0 = ##SYSDRV_BUSDOMAIN_CONFIGURE_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  // Trigger the state update and poll on completion.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR
  set_bits HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_UPDATE_BMSK

  r2 = ##HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_UPDATE_BMSK

bus_rcgr_update:
   r1 = memw(r0)
   r3 = and(r1, r2)
   {
     p0 = cmp.eq(r3, r2)
     if (p0.new) jump:t bus_rcgr_update
   }   

   r0 = ##SYSDRV_SCC_MAIN_CONFIGURE_EXIT
  __update_dbg_marker
  
  // ...End Main Bus clock config setup... 
  
  //*******************************************************************
  // Initialize the CDIV registers so that downstream clocks are not
  // overclocked.
  //******************************************************************* 
 
  r0 = ##HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR
  memw(r0) = ##SCC_QTIMER_TMR_CDIV_DIV_CDIVR

  r0 = ##HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR
  memw(r0) = ##SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR
  
  r0 = ##HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR
  memw(r0) = ##SCC_CDIV_QUPV3_CORE_DIV_CDIVR
  
    r0 = ##HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR
  memw(r0) = ##SCC_CDIV_CCD_DIV_CDIVR
  
    r0 = ##HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_ADDR
  memw(r0) = ##SCC_CDIV_SDC_DIV_CDIVR
  
    r0 = ##HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_ADDR
  memw(r0) = ##SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR
  
    r0 = ##HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_ADDR
  memw(r0) = ##SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR

  barrier
  
  r0 = ##SYSDRV_CDIV_CONFIGURE_EXIT
  __update_dbg_marker
  


  //*******************************************************************
  // Enable QTimer and AHB Clocks
  //*******************************************************************
  r0 = ##HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR
  r1 = ##HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ENABLE_BMSK
  memw(r0) = r1
  
  barrier
  
  r13 = add(PC, #0x8)
  jump clk_enable_check 
  
   r0 = ##SYSDRV_QTIMER_TMR_CLK_ENABLE
  __update_dbg_marker
  
  r0 = ##HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR
  r1 = ##HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_ENABLE_BMSK
  memw(r0) = r1
  
  barrier
  
  r13 = add(PC, #0x8)
  jump clk_enable_check 
  
  r0 = ##SYSDRV_AHB_QTMR_ENABLE_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  // Enable SMEM Clock
  //*******************************************************************
  r0 = ##HWIO_SSC_SCC_SMEM_CBCR_ADDR
  r1 = ##HWIO_SSC_SCC_SMEM_CBCR_CLK_ENABLE_BMSK
  r2 = memw(r0)
  r1 = or(r1,r2)

  memw(r0) = r1
  
  barrier
  
  r13 = add(PC, #0x8)
  jump clk_enable_check 
  
  r0 = ##SYSDRV_SMEMCLK_ENABLE_EXIT
  __update_dbg_marker

  //****************************************************************************
  //
  //                    QDSP6SS PLL INITIAL BRING-UP
  //
  //****************************************************************************

  //*******************************************************************
  // Set up the PLL L-value, CAL_L value, and fractional divider.
  //*******************************************************************
  Q6PLLProg:

  {
    r0 = ##HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR
    r1 = r18
  }
  memw(r0) = r1
  barrier
  
  r0 = ##SYSDRV_SSC_Q6PLL_L_VAL_PROG_EXIT
  __update_dbg_marker

  r0 = ##HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR
  r1 = r20
  memw(r0) = r1
  barrier
  
  r0 = ##SYSDRV_SSC_Q6PLL_CAL_L_VAL_PROG_EXIT
  __update_dbg_marker
  
  r0 = ##HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR
  r1 = r21
  memw(r0) = r1
  barrier

  r0 = ##SYSDRV_SSC_Q6PLL_FRAC_VAL_PROG_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  // Turn off the PLL outputs.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  r1 = memw(r0)
  clear_bits HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK
  
  r0 = ##SYSDRV_SSC_Q6PLL_OUTPUTS_TURNOFF_EXIT
  __update_dbg_marker

  //*******************************************************************
  // Put the PLL into Standby mode.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR
  clear_bits HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_BMSK

  r0 = ##SYSDRV_SSC_Q6PLL_PLL_STANDBY_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  // Bring PLL out of reset.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_SSC_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK
  
  r0 = ##SYSDRV_QDSP6PLL_ENABLE_INTERMEDIATE
  __update_dbg_marker
  
  //*******************************************************************
  // Put the PLL into run mode.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR
  r1 = memw(r0)
  r2 = ##(SSC_PLL_OPMODE_RUN << HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_SHFT)
  r1 = or(r1,r2)
  memw(r0) = r1
  barrier

  r0 = ##SYSDRV_QDSP6PLL_RUN_MODE_EXIT
  __update_dbg_marker
  
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
  
  r0 = ##SYSDRV_QDSP6PLL_LOCK_DET_EXIT
  __update_dbg_marker
  
  //*******************************************************************
  // Enable PLL Outputs
  //*******************************************************************
 
  r0 = ##HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR
  set_bits HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_MAIN_BMSK

  r0 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK

  r0 = ##SYSDRV_QDSP6SS_PLL_BRINGUP_EXIT
  __update_dbg_marker
  
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
  clear_bits HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK
  clear_bits HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK
  r1 = memw(r0)
  {
    r2 = ##(SSC_QDSP6SS_RCG_SRC_SEL_VAL << HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT)
	r3=  ##(SSC_QDSP6SS_RCG_DIV_VAL << HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT)
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  
  barrier

  r0 = #SYSDRV_QDSP6SS_CORE_CLK_CONFIG_EXIT
  __update_dbg_marker
  
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
   
  r0 = ##SYSDRV_Q6_CORE_DOMAIN_EXIT
  __update_dbg_marker
  
   __busywait_deinit
   
  r0 = ##SYSDRV_BUSYWAIT_DEINIT_EXIT
  __update_dbg_marker  
  
end_init:
   Jumpr r31

.size __hexagon_bsp_init, .-__hexagon_bsp_init
#endif // SIM