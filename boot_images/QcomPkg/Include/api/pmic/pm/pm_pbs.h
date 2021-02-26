#ifndef __PM_PBS_H__
#define __PM_PBS_H__
/*! \file 
 *  \n
 *  \brief  pm_pbs.h ---- PMIC PBS driver
 *  \n
 *  \n This header file contains enums, macros and public API definitions for PMIC PBS driver.
 *  \n
 *  \n &copy; Copyright 2013 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/18   ks      Updated NO_OF_PON_RSN_PER_PMIC to hold 5 pon reason history 
03/11/13   kt      Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*=========================================================================
                    Op-code Macros
==========================================================================*/
/**
 * Op-code macros are the instruction command op-code values and 
 * are used in constructing sequence code macros below. User need 
 * not use the op-code macros and can call the below sequence code 
 * macros directly for writing PBS sequences. 
 */
#define PM_PBS_OPCODE_WRITE_REG           0x00
#define PM_PBS_OPCODE_WRITE_REG_SUB       0x04
#define PM_PBS_OPCODE_WRITE_REG_BUF       0x08
#define PM_PBS_OPCODE_WRITE_REG_SUB_BUF   0x0C
#define PM_PBS_OPCODE_READ_REG            0x40
#define PM_PBS_OPCODE_READ_REG_SUB        0x44
#define PM_PBS_OPCODE_GOTO                0x80
#define PM_PBS_OPCODE_GOTO_RELATIVE       0x84
#define PM_PBS_OPCODE_GOTO_SUB            0x88
#define PM_PBS_OPCODE_SKIP_IF             0x90
#define PM_PBS_OPCODE_WAIT                0xC0
#define PM_PBS_OPCODE_INFO                0xF8

/*=========================================================================
                    Sequence code Macros
==========================================================================*/
/**
 * Below are all the macros which can be used as PBS sequence
 * codes. 
 */

/**
 * This macro is used to write to a single register in given 
 * slave id (sid), peripheral id (pid) and register offset 
 * (offset). 
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 *  
 * For example, to write data 0xC0 to 0x88D register, the 
 * sequence macro would be 
 * PM_PBS_WRITE_REG(0x0,0x08,0x8D,0xC0).
 */
#define PM_PBS_WRITE_REG(sid, pid, offset, data)   (((((uint32)PM_PBS_OPCODE_WRITE_REG) | ((sid)&0xFF))<<24) | (((pid)&0xFF)<<16) | (((offset)&0xFF)<<8) | ((data)&0xFF))

/**
 * This macro is used to write to a single register in given 
 * slave id (sid), peripheral id (pid) and register offset 
 * (offset) using data from the local buffer R0. 
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 *  
 * For example, to write data in buffer R0 to 0x88D register, 
 * the sequence macro would be 
 * PM_PBS_WRITE_REG_BUF(0x0,0x08,0x8D). 
 */
#define PM_PBS_WRITE_REG_BUF(sid, pid, offset)   (((((uint32)PM_PBS_OPCODE_WRITE_REG_BUF) | ((sid)&0xFF))<<24) | (((pid)&0xFF)<<16) | (((offset)&0xFF)<<8))

/**
 * This macro is used to read a register in given slave id 
 * (sid), peripheral id (pid) and register offset (offset) 
 * and store the result in local buffer R0. 
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 *  
 * For example, to read data from 0x14B08 register and store it 
 * in R0 buffer, the sequence macro would be 
 * PM_PBS_READ_REG(0x1,0x4B,0x08). 
 */
#define PM_PBS_READ_REG(sid, pid, offset)   (((((uint32)PM_PBS_OPCODE_READ_REG) | ((sid)&0xFF))<<24) | (((pid)&0xFF)<<16) | (((offset)&0xFF)<<8))

/**
 * This macro is used to compare the masked contents of local 
 * data buffer R0 and the provided data using the chosen 
 * operator. if true skip over the next sequence (increment PC 
 * by 8), if false execute the next sequence (increment PC by 
 * 4). 
 * Operators: 
 * 0x00 = Not Equal 
 * 0x01 = Equal 
 * 0x02 = Less than or equal 
 * 0x03 = Less than 
 * 0x04 = Greater than or equal 
 * 0x05 = Greater than
 *  
 * For example, to skip over the next sequence if bit 5 of the 
 * value stored in R0 buffer (mask: 0x20) is equal(Oper: 0x01) 
 * to 0x1 (data: 0x20), the sequence macro would be 
 * PM_PBS_SKIP_IF(0x20,0x01,0x20). Please notice that the data 
 * in this case is not 0x1 and would be 0x20 instead since if 
 * the 5th bit is 1 the data value would be 0x20.
 */
#define PM_PBS_SKIP_IF(mask, oper, data)   ((((uint32)PM_PBS_OPCODE_SKIP_IF)<<24) | (((mask)&0xFF)<<16) | (((oper)&0x07)<<8) | ((data)&0xFF))

/**
 * This macro is used to wait for given time. Unit indicates the
 * time increment for the counter. 
 * Clock Sources: 
 * 0x00 = 19.2 MHz clock cycles 
 * 0x01 = 32 KHz clock cycles 
 *  
 * For example, to wait for 12 32KHz clock cycles, the sequence
 * macro would be PM_PBS_WAIT(1,12) or PM_PBS_WAIT(0x01,0x0C).
 */
#define PM_PBS_WAIT(clk_src, time)   ((((uint32)PM_PBS_OPCODE_WAIT)<<24) | (((clk_src)&0x01)<<16) | ((time)&0xFFFF))

/**
 * This macro is used to go to a new absolute address location 
 * in the PBS logical address map. (2 lsbs ignored since all 
 * instructions are 4 bytes long).
 *  
 * PBS logical address map contains 512 words(starting from 0 to
 * 511) where first 256 words are ROM and last 256 words are 
 * RAM. 
 *  
 * To calculate an absolute PBS address (pbs_abs_addr) for a 
 * specific word, we need to multiply the word number of the 
 * address (in scale of 0 - 511) by 4.
 *  
 * For example, to go to word#500 in PBS address map, the
 * sequence macro would be PM_PBS_GOTO(2000). (multiply 500 by 
 * 4) 
 */
#define PM_PBS_GOTO(pbs_abs_addr)   ((((uint32)PM_PBS_OPCODE_GOTO)<<24) | ((pbs_abs_addr)&0xFFFF))

/**
 * This macro is used to go to a new relative location in the 
 * PBS logical address map. (2's compliment math used).
 *  
 * PBS logical address map contains 512 words(starting from 0 to
 * 511) where first 256 words are ROM and last 256 words are 
 * RAM. 
 *  
 * To calculate an absolute PBS address (pbs_abs_addr) for a 
 * specific word, we need to multiply the word number of the 
 * address (in scale of 0 - 511) by 4. 
 *  
 * To calculate relative PBS address (pbs_rel_addr) for a 
 * specific word, we need to calculate the difference of word 
 * numbers between the new and current PBS address locations and 
 * multiply the difference by 4.
 *  
 * For example, if you are exexuting GOTO_RELATIVE sequence at 
 * word#440 to go to word#500 in PBS address map, the sequence 
 * macro would be PM_PBS_GOTO_RELATIVE(240). (multiply 
 * difference 60 by 4) 
 */
#define PM_PBS_GOTO_RELATIVE(pbs_rel_addr)   ((((uint32)PM_PBS_OPCODE_GOTO_RELATIVE)<<24) | ((pbs_rel_addr)&0xFFFF))

/**
 * This macro is used as End of Sequence.
 */
#define PM_PBS_EOS   0xFCFFFFFF

/*=========================================================================
                    Sub-routine Sequence code Macros
==========================================================================*/
/**
 * Sub-routines are entered when the PM_PBS_GOTO_SUB sequence is executed
 * and they are finished when the PM_PBS_RETURN sequence is executed. No
 * nested sub-routines are supported. Special write and read sequences are 
 * provided below which can be executed in a sub-routine(write and read 
 * sequence macros with the suffix _SUB). These sequences use the stored 
 * Slave ID (sid) and Peripheral ID (pid) values which are passed in as 
 * arguments in the PM_PBS_GOTO_SUB sequence.
 */

/**
 * This macro is used to go to a new absolute address location 
 * in the PBS logical address map and treat the code as a 
 * subroutine. Stores slave id (sid), peripheral id (pid) and PC. 
 * Returns on a PM_PBS_RETURN sequence.  
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 */
#define PM_PBS_GOTO_SUB(sid, pid, pbs_abs_addr)   (((((uint32)PM_PBS_OPCODE_GOTO_SUB) | ((sid)&0xFF))<<24) | (((pid)&0xFF)<<16) | ((pbs_abs_addr)&0xFFFF))

/**
 * This macro should be called in a sub-routine and is used to 
 * write to a single register in given slave id (sid), 
 * peripheral id (pid) and register offset (offset) (uses 
 * stored slave id and peripheral id). 
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 */
#define PM_PBS_WRITE_REG_SUB(offset, data)   ((((uint32)PM_PBS_OPCODE_WRITE_REG_SUB)<<24) | (((offset)&0xFF)<<8) | ((data)&0xFF))

/**
 * This macro should be called in a sub-routine and is used to 
 * read a register and store result in local buffer R0(uses 
 * stored slave id and peripheral id).  
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 */
#define PM_PBS_READ_REG_SUB(offset)   ((((uint32)PM_PBS_OPCODE_READ_REG_SUB)<<24) | (((offset)&0xFF)<<8))

/**
 * This macro should be called in a sub-routine and is used to 
 * write to a single register in given slave id (sid), 
 * peripheral id (pid) and register offset (offset) using 
 * data from the local buffer R0(uses stored slave id and 
 * peripheral id). 
 *  
 * PMIC address is broken down into Slave ID(sid), Peripheral
 * ID (pid) and Register offset (offset). 
 * For example, address 0x12345 is broken into following. 
 * Slave ID (sid): 0x1 
 * Peripheral ID (pid): 0x23 
 * Register offset (offset): 0x45
 */
#define PM_PBS_WRITE_REG_SUB_BUF(offset)   ((((uint32)PM_PBS_OPCODE_WRITE_REG_SUB_BUF)<<24) | (((offset)&0xFF)<<8))

/**
 * This macro should be called in a sub-routine and is used to 
 * return from Subroutine. Used to indicate End of Subroutine. 
 * PC = Stored PC. 
 */
#define PM_PBS_RETURN   0x8CFFFF00

#define NO_OF_PON_RSN_PER_PMIC 10          //5 pon reason History
#define MAX_NO_OF_PMICS 7


/*=========================================================================
                           Enums
==========================================================================*/
/** 
  @enum pm_pbs_trigger_type
  @brief PBS trigger types for which the PBS RAM 
  sequences can be added.
 */
typedef enum
{
   PM_PBS_START_WARM_RESET,
   PM_PBS_START_POFF,
   PM_PBS_BUA_SEQUENCE,
   PM_PBS_GPIO_DTEST1,
   PM_PBS_GPIO_DTEST2,
   PM_PBS_GPIO_DTEST3,
   PM_PBS_GPIO_DTEST4,
   PM_PBS_TRIGGER_INVALID
}pm_pbs_trigger_type;

typedef struct
{
  uint32 pmic_id;
  uint32 pon_data[NO_OF_PON_RSN_PER_PMIC];

}pm_pbs_pon_reason_data_type;

typedef struct
{
  uint32   signature;
  uint16   version;                             /* Starts from 1 and increments if we add more data */
  uint8    max_no_of_pmics;
  uint8    no_of_pon_reasons_per_pmics;
  pm_pbs_pon_reason_data_type   pbs_pon_rsn_hist_data[MAX_NO_OF_PMICS];            /* PON Reasons data   */
} pm_pbs_pon_reason_history_data_type;

/**
  @struct pm_pbs_ram_dataset_type
  @brief Type of data that described information regarding
         PBS User Space region.
  @
 */
typedef struct
{
  char word1; /* Val */
  char word2; /* Offset, Condition  */
  char word3; /* Base Addr, Test Mask */
  char word4; /* Command + SlaveID */
}pm_pbs_ram_dataset_type;


typedef enum
{
    PM_PBS_HEADER_SLEEP_DATASET,
    PM_PBS_HEADER_PON_REASON_DATASET,
    PM_PBS_HEADER_RTRR_DATASET,
    PM_PBS_HEADER_LPG_DATASET,
    PM_PBS_HEADER_SPARE_REG_DATASET,
    PM_PBS_HEADER_INVALID
} pm_pbs_header_dataset_type;


/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** 
 * @name pm_pbs_add_ram_sequences 
 *  
 * @brief This function adds sequences passed by the user to PBS RAM
 *        for a specific trigger type and those sequences get executed
 *        when the corresponding trigger occurs. In case of WARM_RESET,
 *        POFF (Shutdown) and BUA triggers the added RAM sequences will
 *        get added prior to the existing PBS sequences for those trigger
 *        types and then branched to the existing sequence by the API. In
 *        case of DTEST trigger an EOS sequence will be added to the end
 *        of the added custom sequences.
 * 
 * @param[in]  pmic_chip. Primary PMIC chip: 0. Secondary PMIC Chip: 1
 * @param[in]  trigger:  
 *                Valid trigger types (WARM RESET, POFF and so on).
 *                Check pm_pbs_trigger_type enum above for more info.
 * @param[in]  seq_ptr:  
 *                Valid sequences to be added to the PBS RAM. Please
 *                refer to list of valid sequence macros above to be
 *                used to come up with sequences.
 * @param[in]  num_seqs:  
 *                Number of valid sequences user wants to add.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER = Null sequence pointer
 *          passed in.
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not supported.
 *          PM_ERR_FLAG_DATA_VERIFY_FAILURE = if the sequence is not
 *          loaded correctly to PBS RAM or if a valid sequence already
 *          exists at the reserved PBS RAM address space for adding the
 *          sequences.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI errors.
 * 
 * @dependencies This function has to be called after pm_device_init
 *               and before TZ gets initialized.
 */
pm_err_flag_type 
pm_pbs_add_ram_sequences(uint8 pmic_chip, pm_pbs_trigger_type trigger, uint32* seq_ptr, uint8 num_seqs);

/** 
 * @name pm_pbs_get_pon_reason_hist_dataset 
 *  
 * @brief This function reads the pon reasons stored in the pbs ram for 
 *        all the available PMICs and saves in a global variable. This 
 *        will be used to dump the value later during the dload mode. 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not supported.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI errors.
 * 
 */
pm_err_flag_type 
pm_pbs_get_pon_reason_hist_dataset();

/** 
 * @name pm_pbs_get_pon_reason_history 
 *  
 * @brief This function reads the pon reasons stored in the pbs ram
 *        for all the available PMICs and copies the data to 
 *          the memory dump location.  
 * @param[in]  pon_reason_history_ptr:  
 *                Address to the location where the PON reasons should be copied.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not supported.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI errors.
 * 
 */

pm_err_flag_type 
pm_pbs_get_pon_reason_history(pm_pbs_pon_reason_history_data_type *pon_reason_history_ptr );


#endif /* __PM_PBS_H__ */
