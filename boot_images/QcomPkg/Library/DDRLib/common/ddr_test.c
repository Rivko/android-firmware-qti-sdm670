/**
 * @file ddr_test.c
 * @brief
 * DDR Test Framework.
 */


/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/01/15   TW      Modified for 64bit support
06/18/13   SL      Added SDRAM_BOTH for test cases.
05/07/13   wg      fixed klocwork warnings.
11/12/13   SL      Added DDR base address for SR and DPD test.
12/12/12   TW      Modified data and addressline test to restore content for
                   ram dump usecase.
11/12/12   SL      Added DDR base address for thorough read/write test.
08/11/10   SL      Initial version.
================================================================================
                 Copyright 2010,2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_test.h"
#include "ClockBoot.h"
#include "ddr_params.h"
#include "ddr_target.h"

/*==============================================================================
                                  MACROS
==============================================================================*/
/**
 * Infinite loop
 */
#define DDR_TEST_INFINITE_LOOP() do{} while(1)

/**
 * Macro for logging DDR test message.
 */
#define DDR_TEST_LOG_MESSAGE(MESSAGE) \
          do \
          { \
            test_log.message = (MESSAGE); \
          } while (0)

/**
 * Macro for logging DDR test error type.
 */
#define DDR_TEST_LOG_ERR_TYPE(TYPE) \
          do \
          { \
            test_log.error_type = (TYPE); \
          } while (0)

/**
 * Macro for logging address and data pattern where DDR test error occurs.
 */
#define DDR_TEST_LOG_ERR_PATTERN(ADDR, DATA) \
          do \
          { \
            test_log.error_addr = (uint32)(ADDR); \
            test_log.error_data = (uint32)(DATA); \
          } while (0)

/**
 * Macro for delaying for some time.
 * Note: The actual delay time is highly dependent on CPU frequency. Each
 *       iteration of the inner loop executes 4-5 instructions, and each
 *       instruction takes one cycle. 100,000,000 is hardcoded here for
 *       iterations of roughly 1 sec when CPU frequency is 400MHz or so.
 */
#define DDR_TEST_DELAY(COUNT) \
          do \
          { \
            uint32 i, j; \
            for (i = 0; i < (COUNT); i++) \
            { \
              for (j = 0; j < 100000000; j++) \
              { \
                i++; \
              } \
              i -= j; \
            } \
          } while (0)

/**
 * Count of delay time between entering and exiting DDR self refresh.
 */
#define DDR_TEST_SELF_REFRESH_DELAY     10

/**
 * Count of delay time between entering and exiting DDR deep power down.
 */
#define DDR_TEST_DEEP_POWER_DOWN_DELAY  10


/*==============================================================================
                                  DATA
==============================================================================*/
/**
 * Variable where DDR test log is stored, including test message, error type,
 * and address and data pattern where error occurs.
 */
static volatile struct ddr_test_log test_log;

/**
 * Minimum DDR base address for DDR test.
 */
static volatile uint32 min_base = 0;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  Function : ddr_test_mem_map
** =============================================================================
*/
/**
*   @brief
*   Test if memory size in words is power of 2, and base address is aligned with
*   size.
*
*   Memory size in words is power of 2 if and only if size in bytes is power of
*   2 (i.e., (size & (size-1)) == 0x0) and is no smaller than one word. Base
*   address is aligned with size if and only if (base & (size-1)) == 0x0.
*
*   Notice this function takes memory limit as input, where limit = (size-1).
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @retval  TRUE     Test passed
*   @retval  FALSE    Test failed
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/

static boolean ddr_test_mem_map
(
 uint32 base,
 uint32 limit
)
{
  DDR_TEST_LOG_MESSAGE("Testing Memory Map.");

  return ( ((limit + 1) & limit) == 0x0 &&
           limit >= sizeof(uint32) - 1 &&
           (base & limit) == 0x0 );
}

/* =============================================================================
**  Function : ddr_test_data_lines
** =============================================================================
*/
/**
*   @brief
*   Test by walking-ones if none of data lines is open, or is shorted high, low
*   or to another line.
*
*   This function writes (0x1 << n) to the first word of memory and then reads
*   back to check if the same value is fetched, where n is from 0 to
*   (data width - 1). This is for testing if each data line can be set and unset
*   individually. ~(0x1 << n) is written to the last word of memory each time
*   after (0x1 << n) is written, in order to clear data lines in case some line
*   is open.
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @retval  TRUE     Test passed
*   @retval  FALSE    Test failed
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static boolean ddr_test_data_lines
(
  uint64 base,
  uint32 limit
)
{
  volatile uint32 *base_addr;
  uint32 pattern;
  uint32 data0, data1;

  DDR_TEST_LOG_MESSAGE("Testing DDR Read/Write: Data lines.");

  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  /* save the data before writing to it for ram dump case */
  data0 = base_addr[0];
  data1 = base_addr[limit];

  /* test data lines by walking-ones */
  for (pattern = 0x1; pattern != 0x0; pattern <<= 1)
  {
    /* write (0x1 << n) to first word */
    base_addr[0] = pattern;

    /* write ~(0x1 << n) to last word to clear data lines */
    base_addr[limit] = ~pattern;

    /* check if same value is read back */
    if (base_addr[0] != pattern)
    {
      DDR_TEST_LOG_ERR_PATTERN(&base_addr[0], pattern);
      return FALSE;
    }
  }

  /* restore the data for ram dump*/
  base_addr[0] = data0;
  base_addr[limit]= data1;

  return TRUE;
}

/* =============================================================================
**  Function : ddr_test_addr_lines
** =============================================================================
*/
/**
*   @brief
*   Test if none of address lines is open, or is shorted high, low or to another
*   line.
*
*   This function first writes a unique value to each word at the address of
*   (0x1 << n), as well as to the word at 0x0, where n is from 0 to
*   (address width - 1). If address lines are not wired properly, then two
*   writes will go to the same location, which can be detected by checking if a
*   different value is read back from one of these words.
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @retval  TRUE     Test passed
*   @retval  FALSE    Test failed
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static boolean ddr_test_addr_lines
(
  uint64 base,
  uint32 limit
)
{
  volatile uint32 *base_addr;
  uint32 addr_line;
  uint32 data[32] = {0};
  uint32 i;

  DDR_TEST_LOG_MESSAGE("Testing DDR Read/Write: Address lines.");

  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  /* write negation of address to each word at (0x1 << n) */
  for (addr_line = 0x1, i = 1; addr_line <= limit; addr_line <<= 1, i++)
  {
    /* save the address prior to overwriting it */
    data[i] = base_addr[addr_line];
    base_addr[addr_line] = ~addr_line;
  }

  /*
  save content at base */
  data[0] = base_addr[0x0];

  /* write negation of address to word at 0x0 */
  base_addr[0x0] = ~0x0;

  /* check if same value is read back from each word */
  for (addr_line = 0x1, i = 1; addr_line <= limit; addr_line <<= 1, i++)
  {
    if (base_addr[addr_line] != ~addr_line)
    {
      DDR_TEST_LOG_ERR_PATTERN(&base_addr[addr_line], ~addr_line);
      return FALSE;
    }
    else
    {
      /* restore the address line's original content */
      base_addr[addr_line] = data[i];
    }
  }

  /* restore base content */
  base_addr[0x0] = data[0];
  return TRUE;
}

/* =============================================================================
**  Function :  ddr_test_own_addr
** =============================================================================
*/
/**
*   @brief
*   Test memory integrity by own-address algorithm.
*
*   This function writes each word of memory with its own address, and reads
*   back to check if the same value is fetched. It then runs a second write/read
*   pass using the negation of address, in order to test each bit with both "0"
*   and "1".
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @retval  TRUE     Test passed
*   @retval  FALSE    Test failed
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data/address lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static boolean ddr_test_own_addr
(
  uint64 base,
  uint32 limit
)
{
  volatile uint32 *base_addr;
  uint32 offset;

  DDR_TEST_LOG_MESSAGE("Testing DDR Read/Write: Own-address algorithm.");

  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  /* write each word with its own address */
  for (offset = 0; offset <= limit; offset++)
  {
    base_addr[offset] = offset;
  }
  /* check if same value is read back from each word */
  for (offset = 0; offset <= limit; offset++)
  {
    if (base_addr[offset] != offset)
    {
      DDR_TEST_LOG_ERR_PATTERN(&base_addr[offset], offset);
      return FALSE;
    }
  }

  /* write each word with negation of address */
  for (offset = 0; offset <= limit; offset++)
  {
    base_addr[offset] = ~offset;
  }
  /* check if same value is read back from each word */
  for (offset = 0; offset <= limit; offset++)
  {
    if (base_addr[offset] != ~offset)
    {
      DDR_TEST_LOG_ERR_PATTERN(&base_addr[offset], ~offset);
      return FALSE;
    }
  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_test_walking_ones
** =============================================================================
*/
/**
*   @brief
*   Test memory integrity by walking-ones algorithm.
*
*   This function writes "0x1" to the first word of memory, and rotates data
*   pattern one bit left for each write to successive word. These patterns are
*   then verified on subsequent read pass. The start pattern is shifted one bit
*   left for each write/read pass. It runs 32 passes (1 word = 32 bits) in total
*   for all possible data patterns.
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @retval  TRUE     Test passed
*   @retval  FALSE    Test failed
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data/address lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static boolean ddr_test_walking_ones
(
  uint64 base,
  uint32 limit
)
{
  volatile uint32 *base_addr;
  uint32 offset;
  uint32 start_pattern, pattern;

  DDR_TEST_LOG_MESSAGE("Testing DDR Read/Write: Walking-ones algorithm.");

  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  for (start_pattern = 0x1; start_pattern != 0x0; start_pattern <<= 1)
  {
    /* write pass: rotate data pattern one bit left for each successive word */
    pattern = start_pattern;
    for (offset = 0; offset <= limit; offset++)
    {
      base_addr[offset] = pattern;
      pattern <<= 1;
      if (pattern == 0x0)
      {
        pattern = 0x1;
      }
    }

    /* read pass for verification */
    pattern = start_pattern;
    for (offset = 0; offset <= limit; offset++)
    {
      if (base_addr[offset] != pattern)
      {
        DDR_TEST_LOG_ERR_PATTERN(&base_addr[offset], pattern);
        return FALSE;
      }
      pattern <<= 1;
      if (pattern == 0x0)
      {
        pattern = 0x1;
      }
    }
  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_test_access
** =============================================================================
*/
/**
*   @brief
*   Test DDR access.
*
*   This function tests DDR read/write with test cases according to the test
*   level specified by 'level':
*
*   Level 0:   data lines + address lines
*   Level 1:   own-address
*   Level >=2: walking-ones
*
*   @param[in]  level    DDR test level
*   @param[in]  base     Base address of DDR
*   @param[in]  limit    Maximum offset (in bytes) of DDR
*
*   @retval  DDR_TEST_ERR_NONE            Test passed
*   @retval  DDR_TEST_ERR_MEM_MAP         Test failed: memory map error
*   @retval  DDR_TEST_ERR_DATA_LINES      Test failed: data line error
*   @retval  DDR_TEST_ERR_ADDR_LINES      Test failed: address line error
*   @retval  DDR_TEST_ERR_OWN_ADDR        Test failed: own-address error
*   @retval  DDR_TEST_ERR_WALKING_ONES    Test failed: walking-ones error
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static ddr_test_error_type ddr_test_access
(
  uint32 level,
  uint64 base,
  uint32 limit
)
{
  ddr_test_error_type test_error_type = DDR_TEST_ERR_NONE;

  DDR_TEST_LOG_MESSAGE("Testing DDR Read/Write.");

  if (level == 0)
  {
    /* Level 0: data lines + address lines */
    if (!ddr_test_data_lines(base, limit))
    {
      test_error_type = DDR_TEST_ERR_DATA_LINES;
    }
    else if (!ddr_test_addr_lines(base, limit))
    {
      test_error_type = DDR_TEST_ERR_ADDR_LINES;
    }
  }
  else if (level == 1)
  {
    /* Level 1: own-address */
    if (!ddr_test_own_addr(base, limit))
    {
      test_error_type = DDR_TEST_ERR_OWN_ADDR;
    }
  }
  else
  {
    /* Level >=2: walking-ones */
    if (!ddr_test_walking_ones(base, limit))
    {
      test_error_type = DDR_TEST_ERR_WALKING_ONES;
    }
  }

  return test_error_type;
}

/* =============================================================================
**  Function : ddr_test_self_refresh
** =============================================================================
*/
/**
*   @brief
*   Test DDR self refresh.
*
*   This function first tests memory map by checking if memory size in words is
*   power of 2, and base address is aligned with size. If not, error is
*   returned. Otherwise each word of memory is written with its own address, and
*   DDR is put into and taken out of self refresh for a number of iterations.
*   Each word is then read back to check if the same value is fetched.
*
*   @param[in]  iterations     Number of iterations to enter/exit self refresh
*   @param[in]  interface      Interface where DDR is located
*   @param[in]  chip_select    Chip select where DDR is located
*   @param[in]  base           Base address of DDR
*   @param[in]  limit          Maximum offset (in bytes) of DDR
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  DDR_TEST_ERR_NONE            Test passed
*   @retval  DDR_TEST_ERR_MEM_MAP         Test failed: memory map error
*   @retval  DDR_TEST_ERR_SELF_REFRESH    Test failed: self refresh error
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static ddr_test_error_type ddr_test_self_refresh
(
  uint32            iterations,
  DDR_CHANNEL       interface,
  DDR_CHIPSELECT    chip_select,
  uint64            base,
  uint32            limit,
  uint32            clk_speed
)
{
  volatile uint32 *base_addr;
  uint32 offset, addr_mod;
  ddr_info ddr_physical_info = ddr_get_info();

  DDR_TEST_LOG_MESSAGE("Testing DDR Self Refresh.");

  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;

  limit >>= 2;

  /* write each word with its own address */
  DDR_TEST_LOG_MESSAGE("Testing DDR Self Refresh: Write pass.");
  for (offset = 0; offset <= limit; offset++)
  {
    base_addr[offset] = offset;
  }

  while (iterations > 0)
  {
    iterations--;

    /* enter self refresh */
    DDR_TEST_LOG_MESSAGE("Testing DDR Self Refresh: Entering self refresh.");
    ddr_enter_self_refresh(interface, chip_select, clk_speed);

    /* delay for some time */
    DDR_TEST_LOG_MESSAGE("Testing DDR Self Refresh: In self refresh.");
    DDR_TEST_DELAY(DDR_TEST_SELF_REFRESH_DELAY);

    /* exit self refresh */
    DDR_TEST_LOG_MESSAGE("Testing DDR Self Refresh: Exiting self refresh.");
    ddr_exit_self_refresh(interface, chip_select, clk_speed);
  }

  /* check if same value is read back from each word */
  DDR_TEST_LOG_MESSAGE("Testing DDR Self Refresh: Read pass.");
  for (offset = 0; offset <= limit; offset++)
  {
    if(0 != ddr_physical_info.interleaved_memory) {
      /* Channel address check, if not in channel skip read validation, support 2/4 channels */
      addr_mod = (uint32)(&base_addr[offset]) % (DDR_INTERLEAVE_CHANNEL_SIZE * DDR_MAX_NUM_CH);
      if(addr_mod < DDR_INTERLEAVE_CHANNEL_SIZE) {
        /* In CH0 range */
        if( 0 == (interface&DDR_CH0) ) continue;
      }
      else if(addr_mod>=DDR_INTERLEAVE_CHANNEL_SIZE && addr_mod<DDR_INTERLEAVE_CHANNEL_SIZE*2) {
        /* In CH1 range */
        if( 0 == (interface&DDR_CH1) ) continue;
      }
      else if(addr_mod>=DDR_INTERLEAVE_CHANNEL_SIZE*2 && addr_mod<DDR_INTERLEAVE_CHANNEL_SIZE*3) {
        /* In CH2 range */
        if( 0 == (interface&DDR_CH2) ) continue;
      }
      else if(addr_mod>=DDR_INTERLEAVE_CHANNEL_SIZE*3 && addr_mod<DDR_INTERLEAVE_CHANNEL_SIZE*4) {
        /* In CH3 range */
        if( 0 == (interface&DDR_CH3) ) continue;
      }
      else{
        /* More than 4 channels */
        DDR_TEST_LOG_MESSAGE("More than 4 channels, not support!");
        return DDR_TEST_ERR_SELF_REFRESH;
      }
    }
 
    if( base_addr[offset] != offset ) 
    {
      DDR_TEST_LOG_ERR_PATTERN(&base_addr[offset], offset);
      return DDR_TEST_ERR_SELF_REFRESH;
    }
  }

  return DDR_TEST_ERR_NONE;
}

/* =============================================================================
**  Function : ddr_test_deep_power_down
** =============================================================================
*/
/**
*   @brief
*   Test DDR deep power down.
*
*   This function first tests memory map by checking if memory size in words is
*   power of 2, and base address is aligned with size. If not, error is
*   returned. Then DDR is put into and taken out of deep power down for a number
*   of iterations. Memory integrity is tested afterwards.
*
*   @param[in]  iterations     Number of iterations to enter/exit deep power
*                              down
*   @param[in]  interface      Interface where DDR is located
*   @param[in]  chip_select    Chip select where DDR is located
*   @param[in]  base           Base address of DDR
*   @param[in]  limit          Maximum offset (in bytes) of DDR
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  DDR_TEST_ERR_NONE               Test passed
*   @retval  DDR_TEST_ERR_MEM_MAP            Test failed: memory map error
*   @retval  DDR_TEST_ERR_DEEP_POWER_DOWN    Test failed: deep power down error
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

static ddr_test_error_type ddr_test_deep_power_down
(
  uint32            iterations,
  DDR_CHANNEL   interface,
  DDR_CHIPSELECT  chip_select,
  uint64            base,
  uint32            limit,
  uint32            clk_speed
)
{
  volatile uint32 *base_addr;
  uint32 offset;
  
  DDR_TEST_LOG_MESSAGE("Testing DDR Deep Power Down.");
  
  /* Convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;
  
  /* write each word with its own address */
  DDR_TEST_LOG_MESSAGE("Testing DDR Deep Power Down: Write pass.");
  for (offset = 0; offset <= limit; offset++)
  {
    base_addr[offset] = offset;
  }

  while (iterations > 0)
  {
    iterations--;

    /* enter deep power down */
    DDR_TEST_LOG_MESSAGE("Testing DDR Deep Power Down: Entering deep power down.");
    ddr_enter_deep_power_down(interface, chip_select);

    /* delay for some time */
    DDR_TEST_LOG_MESSAGE("Testing DDR Deep Power Down: In deep power down.");
    DDR_TEST_DELAY(DDR_TEST_DEEP_POWER_DOWN_DELAY);

    /* exit deep power down */
    DDR_TEST_LOG_MESSAGE("Testing DDR Deep Power Down: Exiting deep power down.");
    ddr_exit_deep_power_down(interface, chip_select, clk_speed);
  }
  
  /* check if same value is read back from each word */
  DDR_TEST_LOG_MESSAGE("Testing DDR Deep Power Down: Read pass.");
  for (offset = 0; offset <= limit; offset++)
  {
    if (base_addr[offset] != offset)
    {
      DDR_TEST_LOG_ERR_PATTERN(&base_addr[offset], offset);
      return DDR_TEST_ERR_DEEP_POWER_DOWN;
    }
  }
  
  return DDR_TEST_ERR_NONE;
}

/* =============================================================================
**  Function : ddr_test_clock_switching
** =============================================================================
*/
/**
*   @brief
*   Test DDR clock switching.
*
*   This function tests DDR clock switching. We query the clock plan and get 
*   the clock plan based on query. We will request clock by clock and test its
*   working condition/
*
*   @param[in]  base     Base address of DDR
*   @param[in]  limit    Maximum offset (in bytes) of DDR
*
*   @retval  DDR_TEST_ERR_NONE            Test passed
*   @retval  DDR_TEST_ERR_OWN_ADDR        Test failed: own-address error
*
*   @dependencies
*   Memory size in words must be power of 2, and base address must be aligned
*   with size. Also assume data lines are wired properly.
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/
static ddr_test_error_type ddr_test_clock_switching(uint64 base, uint32 limit)
{
	volatile uint32 index1, index2, i;
	ddr_test_error_type test_error_type = DDR_TEST_ERR_NONE;
    uint64 bases[4] = {0x80000000,0xFFE00000,0x100000000,0x1FFDFFFFC};
    limit = 0x200000;
		
	DDR_TEST_LOG_MESSAGE("Testing DDR Clock Switching: Enter DDR Clock Switch.");
	ddr_printf(DDR_NORMAL, "Clock Freq Levels : %u\n", ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels);
		
	for(index1 = 0; index1 < ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels; index1++)
	{
	    if (ddrsns_share_data->aop_params.mc_states[index1].freq_enable == 0)
            continue;
		Clock_SetBIMCSpeed(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[index1].clk_freq_in_khz);
		for (i=0; i<4; i++)
		{
			if (!ddr_test_own_addr(bases[i], limit))
			{
				test_error_type = DDR_TEST_ERR_OWN_ADDR;
			}
		}
		for (index2 = 0; index2 < ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels; index2++) 
		{
			if (ddrsns_share_data->aop_params.mc_states[index2].freq_enable == 0)
				continue;
			Clock_SetBIMCSpeed(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[index2].clk_freq_in_khz);
			ddr_printf(DDR_NORMAL, "Freq Switch From : %u  To : %u\n", ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[index1].clk_freq_in_khz, 
		            ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[index2].clk_freq_in_khz);
			for (i=0; i<4; i++)
			{
				if (!ddr_test_own_addr(base, limit))
				{
					test_error_type = DDR_TEST_ERR_OWN_ADDR;
				}
			}
			Clock_SetBIMCSpeed(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[index1].clk_freq_in_khz);
		}
	}

	DDR_TEST_LOG_MESSAGE("Testing DDR Clock Switching: Exiting DDR Clock Switch.");  

	return DDR_TEST_ERR_NONE;
}

/* =============================================================================
**  Function : ddr_test
** =============================================================================
*/
/**
*   @brief
*   Test DDR located at specific interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  interface      Interface where DDR is located
*   @param[in]  chip_select    Chip select where DDR is located
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed or no DDR attached at the interface/CS
*   @retval  FALSE    Test failed
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

boolean ddr_test
(
  ddr_test_suite    test_suite,
  uint32            test_level,
  DDR_CHANNEL       interface,
  DDR_CHIPSELECT    chip_select,
  uint32            clk_speed
)
{
  uint64 ddr_base = 0, ddr_size = 0;
  ddr_size_info ddr_info_test = ddr_get_size();
  ddr_test_error_type test_error_type = DDR_TEST_ERR_NONE;

  if (chip_select == DDR_CS0)
  {
    ddr_base = ddr_info_test.ddr_cs0_addr[0];
    ddr_size = ddr_info_test.ddr_cs0[0];
  }
  else if (chip_select == DDR_CS1)
  {
    ddr_base = ddr_info_test.ddr_cs1_addr[0];
    ddr_size = ddr_info_test.ddr_cs1[0];
  }
  else if (chip_select == DDR_CS_BOTH)
  {
    ddr_base = (ddr_info_test.ddr_cs0_addr[0] < ddr_info_test.ddr_cs1_addr[0]) ?
                ddr_info_test.ddr_cs0_addr[0] : ddr_info_test.ddr_cs1_addr[0];
    ddr_size = ddr_info_test.ddr_cs0[0] + ddr_info_test.ddr_cs1[0];
  }

  /* convert size from megabyte to byte */
  ddr_size <<= 20;

  if (ddr_size == 0)
  {
    test_error_type = DDR_TEST_ERR_UNATTACHED;
  }
  else if (ddr_base + ddr_size > min_base)
  {
    /* adjust base address and size based on minimum base address for test */
    if (ddr_base < min_base)
    {
      ddr_size -= (min_base - ddr_base);
      ddr_base = min_base;
    }

    /* choose right test suite */
    switch (test_suite)
    {
      case DDR_TEST_SUITE_EMPTY:
      case DDR_TEST_SUITE_MAX:
	  case DDR_CUSTOM_TEST:
        break;

      case DDR_TEST_SUITE_ACCESS:
        test_error_type = ddr_test_access(test_level, ddr_base, ddr_size-1);
        break;

      case DDR_TEST_SUITE_SELF_REFRESH:
        test_error_type = ddr_test_self_refresh(test_level, interface, DDR_CS_BOTH,
                                                ddr_base, ddr_size-1, clk_speed);
        break;

      case DDR_TEST_SUITE_DEEP_POWER_DOWN:
        test_error_type = ddr_test_deep_power_down(test_level, interface, chip_select,
                                                   ddr_base, ddr_size-1, clk_speed);
        break;
	  case DDR_TEST_SUITE_CLOCK_SWITCHING:
		test_error_type = ddr_test_clock_switching(ddr_base, ddr_size-1);
		break;
    }
  }

  /* log test error type */
  DDR_TEST_LOG_ERR_TYPE(test_error_type);

  return ( test_error_type == DDR_TEST_ERR_NONE ||
           test_error_type == DDR_TEST_ERR_UNATTACHED );
}

/**
*   @brief
*   Test DDR located at specific interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  interface      Interface where DDR is located
*   @param[in]  chip_select    Chip select where DDR is located
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed or no DDR attached at the interface/CS
*   @retval  FALSE    Test failed
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

boolean ddr_custom_test
(
  ddr_test_suite    test_suite,
  uint32            test_level,
  uint64 ddr_base,
  uint64 ddr_size,
  uint32            clk_speed
)
{
  ddr_test_error_type test_error_type = DDR_TEST_ERR_NONE;

    switch (test_suite)
    {
      case DDR_TEST_SUITE_EMPTY:
	  case DDR_TEST_SUITE_ACCESS:
      case DDR_TEST_SUITE_MAX:
	  case DDR_TEST_SUITE_SELF_REFRESH:
	  case DDR_TEST_SUITE_DEEP_POWER_DOWN:
	  case DDR_TEST_SUITE_CLOCK_SWITCHING:
        break;

      case DDR_CUSTOM_TEST:
        test_error_type = ddr_test_access(test_level, ddr_base, ddr_size-1);
        break;
    }

  /* log test error type */
  DDR_TEST_LOG_ERR_TYPE(test_error_type);

  return ( test_error_type == DDR_TEST_ERR_NONE ||
           test_error_type == DDR_TEST_ERR_UNATTACHED );
}

/* =============================================================================
**  Function : ddr_test_all
** =============================================================================
*/
/**
*   @brief
*   Test DDR located at each interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed for all attached DDR
*   @retval  FALSE    Test failed for some attached DDR
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

boolean ddr_test_all
(
  ddr_test_suite  test_suite,
  uint32          test_level,
  uint32          clk_speed
)
{
  uint8 cnt_ch, cnt_cs;
  uint8 interface, chip_select;
  ddr_info ddr_physical_info = ddr_get_info();
  
  /* if memory interleave is enabled, check only channel 0 
     as it will interleave across to other channels as well
  */
  for(cnt_cs=0; cnt_cs<2; cnt_cs++) {
    chip_select = 0x1<<cnt_cs;
    if(ddr_physical_info.interleaved_memory & chip_select) {
      if(!ddr_test(test_suite, test_level, DDR_CH0, chip_select, clk_speed))
        return FALSE;
    }
    else {
      for(cnt_ch=0; cnt_ch<DDR_MAX_NUM_CH; cnt_ch++) {
        interface = 0x1<<cnt_ch;
        if(!ddr_test(test_suite, test_level, interface, chip_select, clk_speed))
          return FALSE;
      }
    }
  }
  return TRUE;
}

/* =============================================================================
**  Function : ddr_test_exit
** =============================================================================
*/
/**
*   @brief
*   Exit point of DDR Test Framework: an infinite loop.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/

void ddr_test_exit( void )
{
  DDR_TEST_INFINITE_LOOP();
}
