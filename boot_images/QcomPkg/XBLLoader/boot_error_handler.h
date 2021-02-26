#ifndef BOOT_ERROR_HANDLER_H
#define BOOT_ERROR_HANDLER_H

/*===========================================================================

                       Boot Error Handler Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the BOOT
  error handler.  It is called in the event a BOOT failure is detected.
  Once this boot_error_handler is called. It will never return. 
    
Copyright 2010, 2013-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/23/16   digant  exception context struct to save registers
06/02/16   digant  xpu violation query update
10/29/14   kpa     Support 64 bit memory map
10/21/14   ck      Updated BOOT_VALID_ADDR_INC_IMEM to 64 bit
03/25/14   kedara  Added sbl_pcie_coredump_addr, sbl_pcie_coredump_size
03/18/14   kedara  Moved sbl_save_regs() definition
11/12/13   kedara  Added changes to support SBL crash dumps
07/10/13   aus     Added support to save the registers and flush the L2 TCM to DDR
                   on SBL errors
08/04/10   aus     Clean-up secboot 3.0
05/26/10   plc     Initial port to secboot 3.0 common code  
01/28/10   ty      Initial revision

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#include "boot_error_if.h"

/*==========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/
#define BOOT_CRASH_DUMP_VERSION 1
#define BOOT_CRASH_DUMP_COOKIE 0x45525220 // Cookie "ERR "
#define SBL1_EXC_CONTEXT_VALID_COOKIE 0x45584320 // Cookie "EXC "

typedef enum
{
  ARM_SVC_R0 = 0,
  ARM_SVC_R1,
  ARM_SVC_R2,
  ARM_SVC_R3,
  ARM_SVC_R4,
  ARM_SVC_R5,
  ARM_SVC_R6,
  ARM_SVC_R7,
  ARM_SVC_R8,
  ARM_SVC_R9,
  ARM_SVC_R10,
  ARM_SVC_R11,
  ARM_SVC_R12,
  ARM_SVC_SP,
  ARM_SVC_LR,
  ARM_SVC_SPSR,
  ARM_SVC_PC,
  ARM_CPSR,
  ARM_SYS_SP,
  ARM_SYS_LR,
  ARM_IRQ_SP,
  ARM_IRQ_LR,
  ARM_IRQ_SPSR,
  ARM_ABT_SP,
  ARM_ABT_LR,
  ARM_ABT_SPSR,
  ARM_UDF_SP,
  ARM_UDF_LR,
  ARM_UDF_SPSR,
  ARM_FIQ_R8,
  ARM_FIQ_R9,
  ARM_FIQ_R10,
  ARM_FIQ_R11,
  ARM_FIQ_R12,
  ARM_FIQ_SP,
  ARM_FIQ_LR,
  ARM_FIQ_SPSR,
  SIZEOF_ARM_REGISTERS
} register_type;

typedef struct
{
  uint32 crash_dump_cookie;            // Crash dump magic cookie
  uint32 version;                      // Version of the data structure
  uintnt regs[SIZEOF_ARM_REGISTERS];   // Register array
  uintnt sbl_ddr_dump_addr;            // Location in DDR where SBL DDR region is dumped
  uintnt sbl_ddr_addr;                 // Address of SBL DDR region
  uintnt sbl_ddr_size;                 // Size of SBL DDR region
  uintnt sbl_l2_dump_addr;             // Location in DDR where SBL L2 region is dumped
  uintnt sbl_l2_addr;                  // Address of SBL L2 region
  uintnt sbl_l2_size;                  // Size of SBL L2 region
  uintnt sbl_pcie_coredump_addr;       // Address of PCIe register dump
  uintnt sbl_pcie_coredump_size;       // Size of PCIe register dump region  
} boot_crash_dump_type;

typedef struct
{
  uint64 exc_cookie;     // Exception context dump magic cookie
  uint64 x0;                           // X0 register contents
  uint64 x1;                           // X1 register contents
  uint64 x2;                           // X2 register contents
  uint64 x3;                           // X3 register contents
  uint64 x4;                           // X4 register contents
  uint64 x5;                           // X5 register contents
  uint64 x6;                           // X6 register contents
  uint64 x7;                           // X7 register contents  
  uint64 x8;                           // X8 register contents
  uint64 x9;                           // X9 register contents
  uint64 x10;                          // X10 register contents
  uint64 x11;                          // X11 register contents
  uint64 x12;                          // X12 register contents
  uint64 x13;                          // X13 register contents
  uint64 x14;                          // X14 register contents
  uint64 x15;                          // X15 register contents
  uint64 x16;                          // X16 register contents
  uint64 x17;                          // X17 register contents
  uint64 x18;                          // X18 register contents
  uint64 x19;                          // X19 register contents
  uint64 x20;                          // X20 register contents
  uint64 x21;                          // X21 register contents
  uint64 x22;                          // X22 register contents
  uint64 x23;                          // X23 register contents  
  uint64 x24;                          // X24 register contents
  uint64 x25;                          // X25 register contents
  uint64 x26;                          // X26 register contents
  uint64 x27;                          // X27 register contents
  uint64 x28;                          // X28 register contents
  uint64 x29;                          // X29 register contents
  uint64 x30;                          // X30 register contents
  uint64 elr;                          // ELR_EL1 exception link register contents
  uint64 esr;                          // ESR_EL1 exception syndrome reg contents
} exc_context;

extern exc_context exception_context;
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
       
/*===========================================================================

**  Function :  boot_err_fatal

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls the SBL error handler.
*   
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_err_fatal( void );


/*===========================================================================

**  Function :  boot_init_stack_chk_canary

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize stack protection canary to a random number  
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_init_stack_chk_canary(void);


/*===========================================================================

**  Function :  sbl_error_handler

** ==========================================================================
*/
/*!
* 
* @brief
*   SBL specific error handling  
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl_error_handler(void);

/*===========================================================================
**  Function :  sbl_save_regs
** ==========================================================================
*/
/*!
* 
* @brief
*   Save current registers at the time of crash
*
* @par Dependencies
*  None        
*   
*/
void sbl_save_regs(void);

/*===========================================================================
**  Function :  boot_query_xpu_violation
** ==========================================================================
*/
/*!
* 
* @brief
*   Function queries the status of xpu violation when serror occurs
*   and logs the information provided by TZ if xpu violation occured
* 
* @param[in]
*	None
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_query_xpu_violation( void );

#endif

/*===========================================================================

**  Macro :  boot_valid_addr_inc_imem 

** ==========================================================================
*/
/*!
* 
* @brief
*   This macro validates the address passed in to insure that it is within
*   the address space that SDRAM or IMEM can exist on the
*   MSM.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   Calls the SBL error handler if the address falls outside the valid range.
* 
*/
#ifndef BOOT_VALID_ADDR_INC_IMEM
#define BOOT_VALID_ADDR_INC_IMEM( addr, error_type ) \
           BL_VERIFY( (((uintnt)(addr) <= RANGE_END_ADDR)) || \
                      (((uintnt)(addr) < IMEM_RANGE_END_ADDR) && \
                       ((uintnt)(addr) >= IMEM_RANGE_START_ADDR)), error_type )
#endif


#endif  /* BOOT_ERROR_HANDLER_H */
