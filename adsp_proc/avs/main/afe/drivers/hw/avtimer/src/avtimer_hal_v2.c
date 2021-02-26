/*========================================================================
avtimer.c

This file contains avtimer hardware abstraction layer.

Copyright (c) 2010-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_hal_v2.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "avtimer_hal_v2.h"
#include "LPASS_ADDRESS_AVTIMER_V2.h"

#include "qurt.h"
#include "qube.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

#define TIME_SAMPLE_SET 1
#define TIME_SAMPLE_RESET 0

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

static const uint64_t AVTIMER_MAX_INTERRUPT_MATCH_VALUE = (uint64_t)(0xffffffffffffLLU);

/* 56-bit AV timer counter */
static const uint32_t AVTIMER_MAX_TICK_VALUE_MSB = (uint32_t)(0xFFFFFFLU);    /* Upper 24-bit */
static const uint32_t AVTIMER_MAX_TICK_VALUE_LSB = (uint32_t)(0xFFFFFFFFLU);  /* Lower 32-bit */

extern void avtimer_server_send_interrupt_signal(void);

#define AVT_OFFSET(_reg_) ((_reg_)- LPASS_AVTIMER_HW_VERSION) 

static volatile uint32   avtimer_base_phy_addr = 0;
static volatile int32    avtimer_virt_offset = 0; 
static volatile int32    atime1_virt_offset = 0; 
static volatile int32    atime2_virt_offset = 0; 

static uint16_t avtimer_l2_int_0_num = 0;

#ifdef __cplusplus
extern "C" {
#endif
void qurt_isr_callback2(int ist_arg) __attribute__ ((alias ("hal_avtimer_ist_int0_cbfunc_v2")));
#ifdef __cplusplus
}
#endif
/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/**< avtimer interrupt 0 callback function */
static void hal_avtimer_ist_int0_cbfunc_v2(int ist_arg);

static uint32 get_avtimer_hw_instance_base_virt_offset_v2(avtimer_hw_index idx);

/**
 * This function must be called to initialize avtimer.
 * The clock source used for AV timer of 8960: PXO
 * The same XO should be used for audio synchronizer.
 */
ADSPResult hal_avtimer_hw_init_v2(avtimer_hw_cfg_t *avt_hw_cfg)
{
   avtimer_base_phy_addr = avt_hw_cfg->avtimer_base_phys_addr;       
   ///< Calculate offset of virtual address in words (4 bytes)    
   avtimer_virt_offset = reg_virt_offset(avt_hw_cfg->avtimer_base_virt_addr, avt_hw_cfg->avtimer_base_phys_addr,
                                         avt_hw_cfg->avtimer_instance_info[AVTIMER_HW_INSTANCE_INDEX_0].offset_addr);

   atime1_virt_offset = reg_virt_offset(avt_hw_cfg->avtimer_base_virt_addr, avt_hw_cfg->avtimer_base_phys_addr,
                                        avt_hw_cfg->avtimer_instance_info[AVTIMER_HW_INSTANCE_INDEX_1].offset_addr);

   atime2_virt_offset = reg_virt_offset(avt_hw_cfg->avtimer_base_virt_addr, avt_hw_cfg->avtimer_base_phys_addr,
                                        avt_hw_cfg->avtimer_instance_info[AVTIMER_HW_INSTANCE_INDEX_2].offset_addr);

   avtimer_l2_int_0_num = (uint16_t) avt_hw_cfg->avtimer_l2_int_0_num;

   return ADSP_EOK;
}

ADSPResult hal_avtimer_hw_enable_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode)
{
///Note to know
//0. which avtimer: u_ATIMER or U_A_TIME [source from pull-able PLL for avsync]
//1. Which mode: EN_QCOUNTER
//               MN_MODE
//     TIMER_2X_RATE@AVTIMER_MODE_CTL can be hard-coded in hal

    ADSPResult rc = ADSP_EOK;

    uint32_t avtimer_temp_value;
    uint32_t temp_base_virt_offset;

    //Tie avtimer interrupt to INSTANCE 0 only
    //Will move to driver layer
    if(AVTIMER_HW_INSTANCE_INDEX_0 == idx)
    {
       if(QURT_EOK != (rc = qurt_isr_register((int)avtimer_l2_int_0_num, hal_avtimer_ist_int0_cbfunc_v2)))
       {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to register avtimer interrupt 0 with status %d\n",rc);
       }
       else
       {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "registered avtimer interrupt 0 with status %d\n",rc);
       }
    }

    temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

    //1. disable and reset timer
    //   apply for all modes
    avtimer_temp_value = ((0 << AVTIMER_CTL__TIMER_EN___S)  | (1 << AVTIMER_CTL__TIMER_RST___S));   //disable timer and reset timer value to zero
    UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_CTL), temp_base_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0); 

    if(AVTIMER_HW_INSTANCE_INDEX_0 == idx)
    {
       //clear & disable int0
       avtimer_temp_value = 1 << AVTIMER_INT0_ST__INT0_CLR___S;
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_ST), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0); 

       avtimer_temp_value = (1 << AVTIMER_INT0_CTL__INT0_MENW___S) | (0 << AVTIMER_INT0_CTL__INT0_ENW___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0); 

       //clear & disable int1
       avtimer_temp_value = 1 << AVTIMER_INT1_ST__INT1_CLR___S;
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_ST), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);   

       avtimer_temp_value = (1 << AVTIMER_INT1_CTL__INT1_MENW___S) | (0 << AVTIMER_INT1_CTL__INT1_ENW___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);   
    }

    //set the match value of int0,int1 to max initially.
    hal_avtimer_prog_hw_timer_int0_v2(idx, hw_mode, AVTIMER_MAX_INTERRUPT_MATCH_VALUE);
    hal_avtimer_prog_hw_timer_int1_v2(idx, hw_mode, AVTIMER_MAX_INTERRUPT_MATCH_VALUE);

    if(AVTIMER_HW_MODE_N_ALPAH == hw_mode)
    {
       //
       //program avtimer_resmod(35.89MHz)
       //avtimer_temp_value = 0x23e3d70; 
       
       //program avtimer_resmod(27MHz)
       //avtimer_temp_value = 0x1B00000;   

       //program avtimer_resmod(19.2MHz)  :  19=0x13,    0.2 => 0.2*2^20 = 209715.2 => 209715 = 0x33333
       avtimer_temp_value = 0x1333333; 
           
       hal_avtimer_prog_hw_timer_resmod_v2(idx, avtimer_temp_value);
    }
    else if(AVTIMER_HW_MODE_L_MN == hw_mode)
    {
       switch(idx)
       {
          case AVTIMER_HW_INSTANCE_INDEX_0:
          default:
          {
             //1. Set L+M/N mode via LPASS_AVTIMER_MODE_CTL and enable 2X
             avtimer_temp_value = (1 << AVTIMER_MODE_CTL__MN_MODE___S) | (1 << AVTIMER_MODE_CTL__TIMER_2X_RATE___S);
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_MODE_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);   

             //HW instance 0 (U_AVTIMER) is used to generate STC 
             //L+M/N -- 13.5 MHz hw ticks --> 27 Mhz via 2X enabled
             //hw ticks = CXO/(L + M/N)
             //CXO = 19.2 MHz
             //L = 1  M = 19  N = 45   Note: M must be < N
             //hw ticks = 13.5 MHz ---> 27 MHz via 2X enabled
             avtimer_temp_value = 1 << AVTIMER_L0_VAL_NEXT__L0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_L0_VAL_NEXT), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

             avtimer_temp_value = 19 << AVTIMER_M0_VAL_NEXT__M0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_M0_VAL_NEXT), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

             avtimer_temp_value = 45 << AVTIMER_N0_VAL_NEXT__N0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_N0_VAL_NEXT), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

             //load L/M/N now 
             avtimer_temp_value = 0xF << AVTIMER_LMN_DWT__UPDATE_EN___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_LMN_DWT), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

          }
          break;
          case AVTIMER_HW_INSTANCE_INDEX_1:
          {
             //1. Set L+M/N mode via LPASS_AVTIMER_MODE_CTL and disable 2X
             avtimer_temp_value = (1 << AVTIMER_MODE_CTL__MN_MODE___S) | (0 << AVTIMER_MODE_CTL__TIMER_2X_RATE___S);
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_MODE_CTL), atime1_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);   

             //to-do for U_A_TIME  [source from pull-able PLL for avsync]
             //Clk Source = LPAAUDIO_PLL (903.168 MHz, should get this from device cfg file)
             //ATIME clk source = 28224000 = 903.168/32   (Div = 1, M = 1, N = 32)
             //out tick = 27000000
             //27000000 = 28224000/(L + M/N)
             //L = 1   M = 153   N = 3375

             avtimer_temp_value = 1 << AVTIMER_L0_VAL_NEXT__L0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_L0_VAL_NEXT), atime1_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

             avtimer_temp_value = 153 << AVTIMER_M0_VAL_NEXT__M0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_M0_VAL_NEXT), atime1_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

             avtimer_temp_value = 3375 << AVTIMER_N0_VAL_NEXT__N0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_N0_VAL_NEXT), atime1_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0); 

             //load L/M/N now 
             avtimer_temp_value = 0xF << AVTIMER_LMN_DWT__UPDATE_EN___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_LMN_DWT), atime1_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

          }
          break;
          case AVTIMER_HW_INSTANCE_INDEX_2:
          {
             //1. Set L+M/N mode via LPASS_AVTIMER_MODE_CTL and disable 2X
             avtimer_temp_value = (1 << AVTIMER_MODE_CTL__MN_MODE___S) | (0 << AVTIMER_MODE_CTL__TIMER_2X_RATE___S);
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_MODE_CTL), atime2_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);   

             //to-do for U_A_TIME  [source from pull-able PLL for avsync]
             //Clk Source = LPAAUDIO_PLL (903.168 MHz, should get this from device cfg file)
             //ATIME clk source = 28224000 = 903.168/32   (Div = 1, M = 1, N = 32)
             //out tick = 27000000
             //27000000 = 28224000/(L + M/N)
             //L = 1   M = 153   N = 3375

             avtimer_temp_value = 1 << AVTIMER_L0_VAL_NEXT__L0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_L0_VAL_NEXT), atime2_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

             avtimer_temp_value = 153 << AVTIMER_M0_VAL_NEXT__M0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_M0_VAL_NEXT), atime2_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0); 

             avtimer_temp_value = 3375 << AVTIMER_N0_VAL_NEXT__N0_VAL_NEXT___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_N0_VAL_NEXT), atime2_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0); 

             //load L/M/N now 
             avtimer_temp_value = 0xF << AVTIMER_LMN_DWT__UPDATE_EN___S;
             UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_LMN_DWT), atime2_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

          }
          break;
       }
    }
    else
    {
       //1. Set source from QCOUNTER via LPASS_AVTIMER_MODE_CTL
       //   The tick generated from hw will be 19.2 MHz
       avtimer_temp_value = (1 << AVTIMER_MODE_CTL__EN_QCOUNTER___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_MODE_CTL), temp_base_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  
    }

    if(AVTIMER_HW_INSTANCE_INDEX_0 == idx)
    {
        //clear & enable int0
       avtimer_temp_value = 1 << AVTIMER_INT0_ST__INT0_CLR___S;
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_ST), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);   

       avtimer_temp_value = (1 << AVTIMER_INT0_CTL__INT0_MENW___S) | (1 << AVTIMER_INT0_CTL__INT0_ENW___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

       //clear & enable int1
       avtimer_temp_value = 1 << AVTIMER_INT1_ST__INT1_CLR___S;
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_ST), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);

       avtimer_temp_value = (1 << AVTIMER_INT1_CTL__INT1_MENW___S) | (1 << AVTIMER_INT1_CTL__INT1_ENW___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  
    }

    //enable and release the reset timer
    avtimer_temp_value = ((1 << AVTIMER_CTL__TIMER_EN___S)  | (0 << AVTIMER_CTL__TIMER_RST___S));   //enable timer and release reset timer 
    UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_CTL), temp_base_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hal_avtimer_hw_init_v2_1 completed (HW Instance, 0x%x  Status,0x%x)", idx, rc);

    return rc;
}

ADSPResult hal_avtimer_hw_disable_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode)
{
///Note to know
//0. which avtimer: u_ATIMER or U_A_TIME [source from pull-able PLL for avsync]
//1. Which mode: EN_QCOUNTER
//               MN_MODE

    uint32_t avtimer_temp_value;
    uint32_t temp_base_virt_offset;
    ADSPResult rc = ADSP_EOK;

    //first set match value to max value to prevent interrupts happening.
    hal_avtimer_prog_hw_timer_int0_v2(idx, hw_mode, AVTIMER_MAX_INTERRUPT_MATCH_VALUE);
    hal_avtimer_prog_hw_timer_int1_v2(idx, hw_mode, AVTIMER_MAX_INTERRUPT_MATCH_VALUE);

    temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

    //disable and reset timer
    avtimer_temp_value = ((0 << AVTIMER_CTL__TIMER_EN___S)  | (1 << AVTIMER_CTL__TIMER_RST___S));   //disable timer and reset timer value to zero
    UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_CTL), temp_base_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

    if(AVTIMER_HW_INSTANCE_INDEX_0 == idx)
    {
       //clear & disable int0
       avtimer_temp_value = 1 << AVTIMER_INT0_ST__INT0_CLR___S;
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_ST), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

       avtimer_temp_value = (1 << AVTIMER_INT0_CTL__INT0_MENW___S) | (0 << AVTIMER_INT0_CTL__INT0_ENW___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

       //clear & disable int1
       avtimer_temp_value = 1 << AVTIMER_INT1_ST__INT1_CLR___S;
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_ST), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

       avtimer_temp_value = (1 << AVTIMER_INT1_CTL__INT1_MENW___S) | (0 << AVTIMER_INT1_CTL__INT1_ENW___S);
       UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_CTL), avtimer_virt_offset, 0xFFFFFFFF, avtimer_temp_value, 0);  

       //now the interrupts must be stopped .
       if(QURT_EOK != (rc = qurt_isr_deregister((int)avtimer_l2_int_0_num)))
       {
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to deregister avtimer int0 (Status,0x%x)", rc);
       }
       else
       {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "deregistered avtimer interrupt 0 with status %d\n",rc);
       }
    }

    return rc;
}


static void hal_avtimer_ist_int0_cbfunc_v2(int ist_arg)
{

   uint32_t avt_reg;

   /* Set the target time to max, so that interrupt doesn't fire */
   /* First write the msb, since the writing to msb and lsb is not atomic. */
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_MSB), avtimer_virt_offset, 0xFFFFFFFF, (uint32_t)(AVTIMER_MAX_TICK_VALUE_MSB), 0);  
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_LSB), avtimer_virt_offset, 0xFFFFFFFF, (uint32_t)(AVTIMER_MAX_TICK_VALUE_LSB), 0);  

   /* Clear the int0 @ source */
   ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_ST), avtimer_virt_offset, &avt_reg);
   avt_reg = avt_reg | AVTIMER_INT0_ST__INT0_CLR___M;
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_ST), avtimer_virt_offset, 0xFFFFFFFF, avt_reg, 0); 

   /* Read back same register again */
   ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_ST), avtimer_virt_offset, &avt_reg);

   avtimer_server_send_interrupt_signal();
}

//Note int0_match_value is always basing on 1 MHz ticks
void hal_avtimer_prog_hw_timer_int0_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int0_match_value)
{
   //0. which avtimer: u_ATIMER or U_A_TIME [source from pull-able PLL for avsync]
   volatile uint32_t temp_base_virt_offset;

   switch (hw_mode)
   {
      case AVTIMER_HW_MODE_N_ALPAH:
      default:
         //hw tick is 1 MHz
         break;
      case AVTIMER_HW_MODE_L_MN:
         //hw tick is 27 MHz
         int0_match_value = (uint64_t)(int0_match_value*27ull);
         break;
      case AVTIMER_HW_MODE_QCOUNTER:
         //hw tick is 19.2 MHz
         int0_match_value = (uint64_t)(int0_match_value*192ull/10ull);
         break;
   }

   temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

   /* First write the lsb, to ensure that interrupt is not enabled until MSB is updated which is set to MAX in intr CB */
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_LSB),
                   temp_base_virt_offset,
                   0xFFFFFFFF,
                   (uint32_t)int0_match_value,
                   0); 
      
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT0_MSB),
                   temp_base_virt_offset,
                   0xFFFFFFFF,
                   (uint32_t)(int0_match_value >> 32),
                   0);  
}

//Note int1_match_value is always basing on 1 MHz ticks
void hal_avtimer_prog_hw_timer_int1_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int1_match_value)
{
//0. which avtimer: u_ATIMER or U_A_TIME [source from pull-able PLL for avsync]
   volatile uint32_t temp_base_virt_offset;

   switch (hw_mode)
   {
      case AVTIMER_HW_MODE_N_ALPAH:
      default:
         //hw tick is 1 MHz
         break;
      case AVTIMER_HW_MODE_L_MN:
         //hw tick is 27 MHz
         int1_match_value = (uint64_t)(int1_match_value*27ull);
         break;
      case AVTIMER_HW_MODE_QCOUNTER:
         //hw tick is 19.2 MHz
         int1_match_value = (uint64_t)(int1_match_value*192ull/10ull);
         break;
   }

   temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

    //first write the msb, since the writing to msb and lsb is not atomic.
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_MSB), temp_base_virt_offset, 0xFFFFFFFF, (uint32_t)(int1_match_value >>32), 0);  
   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_INT1_LSB), temp_base_virt_offset, 0xFFFFFFFF, (uint32_t)int1_match_value, 0);  

   
}

uint64_t hal_avtimer_get_current_hw_timer_tick_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode)
{
//0. which avtimer: u_ATIMER or U_A_TIME [source from pull-able PLL for avsync]
//1. which mode

   uint32_t lsb32 = 0, msb32_1st = 0, msb32_2nd = 0;
   uint64_t curr_tick __attribute__((aligned(8))) = 0;
   volatile uint32_t temp_base_virt_offset;

   temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

   /*
    * LSB overflow cases.
    * 1. msb read just before lsb overflow, lsb read just before overflow.
    * 2. msb read just before lsb overflow, lsb read right after overflow. (we need to read updated msb)
    * 3. msb read after lsb overflow, lsb read after lsb overflow.
    */

   do
   {
      ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_TIMER_MSB), temp_base_virt_offset, &msb32_1st);
      ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_TIMER_LSB), temp_base_virt_offset, &lsb32);
      ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_TIMER_MSB), temp_base_virt_offset, &msb32_2nd);

   }while(msb32_1st != msb32_2nd);

   curr_tick = ((uint64_t)msb32_1st<<32)|(lsb32);

   switch (hw_mode)
   {
      case AVTIMER_HW_MODE_N_ALPAH:
      default:
         //hw tick is 1 MHz
         break;
      case AVTIMER_HW_MODE_L_MN:
         //hw tick is 27 MHz
         curr_tick = (uint64_t)(curr_tick/27ull);
         break;
      case AVTIMER_HW_MODE_QCOUNTER:
         //hw tick is 19.2 MHz
         curr_tick = (uint64_t)(curr_tick*10ull/192ull);
         break;
   }

   return curr_tick;
}

/**
Converts hw tick count to time

@param[in] hw_mode      AV timer Hardware mode
@param[in] tick_count   Hardware tick count

@return
 returns time.
*/
uint64_t hal_avtimer_convert_hw_tick_to_time_v2(avtimer_hw_mode hw_mode, uint64_t tick_count)
{
  uint64_t  timestamp = 0;

    if(AVTIMER_HW_MODE_L_MN == hw_mode)
    {
      timestamp = tick_count/27;  //27 MHz tick
    }
    else if( AVTIMER_HW_MODE_QCOUNTER == hw_mode)
    {
      timestamp = (tick_count * 10)/192; //Qcounter mode (19.2 MHz tick)
    }
    else if( AVTIMER_HW_MODE_N_ALPAH  == hw_mode)
    {
      timestamp = tick_count;
    }

    return timestamp;
}

/**
latches avtimer and qtimer timestamps together

@return
 returns ADSPResult.
*/
ADSPResult hal_avtimer_latch_av_qtimer_tick_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t *avtimer_ts,
                                               uint64_t *qtimer_ts)
{

   uint32_t lsb32 = 0, msb32 = 0;
   uint64_t avt_curr_tick __attribute__((aligned(8))) = 0;
   uint64_t qt_curr_tick __attribute__((aligned(8))) = 0;
   uint32_t temp_base_virt_offset;

   temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

   /*
    * Change register LPASS_AVTIMER_TIME_CTL.TIME_SAMPLE value from 0->1->0.
    * It takes snapshot of Qtimer in LPASS_AVTIMER_EXT_STC_LSB/MSB and
    * Snapshot of AV timer in LPASS_AVTIMER_LTIME_LSB/MSB
    */

   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_TIME_CTL), temp_base_virt_offset,
                         AVTIMER_TIME_CTL__TIME_SAMPLE___M, ((uint32_t)TIME_SAMPLE_RESET << AVTIMER_TIME_CTL__TIME_SAMPLE___S), 0);

   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_TIME_CTL), temp_base_virt_offset,
                   AVTIMER_TIME_CTL__TIME_SAMPLE___M, ((uint32_t)TIME_SAMPLE_SET << AVTIMER_TIME_CTL__TIME_SAMPLE___S), 0);

   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_TIME_CTL), temp_base_virt_offset,
                      AVTIMER_TIME_CTL__TIME_SAMPLE___M, ((uint32_t)TIME_SAMPLE_RESET << AVTIMER_TIME_CTL__TIME_SAMPLE___S), 0);

   ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_LTIME_MSB), temp_base_virt_offset, &msb32);
   ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_LTIME_LSB), temp_base_virt_offset, &lsb32);

   avt_curr_tick = ((uint64_t)msb32<<32)|(lsb32);

   ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_EXT_STC_MSB), temp_base_virt_offset, &msb32);
   ReadRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_EXT_STC_LSB), temp_base_virt_offset, &lsb32);

   qt_curr_tick = ((uint64_t)msb32<<32)|(lsb32);

   switch (hw_mode)
   {
      case AVTIMER_HW_MODE_N_ALPAH:
      default:
         //hw tick is 1 MHz
         break;
      case AVTIMER_HW_MODE_L_MN:
         //hw tick is 27 MHz
        avt_curr_tick = (uint64_t)(avt_curr_tick/27ull);
         break;
      case AVTIMER_HW_MODE_QCOUNTER:
         //hw tick is 19.2 MHz
        avt_curr_tick = (uint64_t)(avt_curr_tick*10ull/192ull);
         break;
   }

   qt_curr_tick = (qt_curr_tick *10ull)/192ull;

   *avtimer_ts = avt_curr_tick;
   *qtimer_ts = qt_curr_tick;

   return ADSP_EOK;
}

void hal_avtimer_prog_hw_timer_resmod_v2(avtimer_hw_index idx, uint32_t resmod_value)
{
//0. which avtimer: u_ATIMER or U_A_TIME [source from pull-able PLL for avsync]
//latency mode only
   uint32_t temp_base_virt_offset;

   temp_base_virt_offset = get_avtimer_hw_instance_base_virt_offset_v2(idx);

   UpdateRegister1(avtimer_base_phy_addr + AVT_OFFSET(LPASS_AVTIMER_RESMOD), temp_base_virt_offset, 0xFFFFFFFF, resmod_value, 0);  
}



static uint32 get_avtimer_hw_instance_base_virt_offset_v2(avtimer_hw_index idx)
{
    uint32_t temp_base_virt_offset;

    //get the base virtual address
    switch(idx)
    {
    case AVTIMER_HW_INSTANCE_INDEX_0:
       temp_base_virt_offset = avtimer_virt_offset;
       break;
    case AVTIMER_HW_INSTANCE_INDEX_1:
       temp_base_virt_offset = atime1_virt_offset;
       break;
    case AVTIMER_HW_INSTANCE_INDEX_2:
       temp_base_virt_offset = atime2_virt_offset;
       break;
    default:
       temp_base_virt_offset = avtimer_virt_offset;
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed in hal_avtimer_hw_enable_v2_1() with wrong idx %d\n",idx);
       break;
    }

    return temp_base_virt_offset;
}
