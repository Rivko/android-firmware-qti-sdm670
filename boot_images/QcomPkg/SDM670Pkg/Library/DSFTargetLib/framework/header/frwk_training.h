#ifndef FRWK_TRAINING_H
#define FRWK_TRAINING_H

/*=============================================================================

                             DDRSNS V3 Framework

GENERAL DESCRIPTION
  DDRSNS V3 Framework header

Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/16   tw      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "HAL_SNS_DDR.h"
//#include "ddrss_training.h"
#include "ddrss.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define MAX_STRING_SIZE 32
#define MAX_DEPENDENCIES 5
#define MANDATORY 1
#define NOT_MANDATORY 0

/* This is a training function pointer which points to every training function and their signature will be like below */
typedef void (*training_func_ptr)
(
  BOOT_DDR_STRUCT *ddr,
  DDR_CHANNEL channel, 
  DDR_CHIPSELECT chip_select,
  uint32 clk_freq_khz,
  uint8 prfs_index, 
  training_params_t *training_params_ptr,
  uint8             *local_vars
);

typedef struct 
{
  uint32 index;                                     /* Struct index in the array */
  char func_name[MAX_STRING_SIZE];                  /* short string name */
  training_func_ptr func_ptr; 	                    /* function pointer for training */
  boolean trng_req;                                 /* Mandatory or optional training */
  training_func_ptr dependencies[MAX_DEPENDENCIES]; /* array of function pointers for training dependencies */
  
}frwk_training_struct;

extern frwk_training_struct pre_training[];
extern frwk_training_struct do_training[];
extern frwk_training_struct post_training[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  frwk_do_trainings

** ==========================================================================
*/
/*!
* 
* @brief
*    given training lists execute them with clock switching requirements
* 
* @param[in] ddr            			      Pointer to the sns shared data
* @param[in] pre_training_procs         Pointer to the function pointer table  
* @param[in] training_procs		          Pointer to the function pointer table 
* @param[in] post_training_procs        Pointer to the function pointer table 
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
void frwk_do_trainings
(
  BOOT_DDR_STRUCT *ddr,
  frwk_training_struct *pre_training_struct,
  frwk_training_struct *do_training_struct,
  frwk_training_struct *post_training_struct,
	training_params_t    *training_params_ptr,
  uint8                *local_vars  
);

/*===========================================================================

**  Function :  frwk_loop_training_seq

** ==========================================================================
*/
/*!
* 
* @brief
*    Processes an array of procedures.
* 
* @param[in] ddr            Pointer to the sns shared data
* @param[in] proc           Pointer to the function pointer table 
* @param[in] array_size     Size of the function pointer table array
* @param[in] freq	    	    Training frequency
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
void frwk_loop_training_seq
(
  BOOT_DDR_STRUCT *ddr,
  frwk_training_struct *proc,
  uint32 array_size,
  uint32 freq,
  uint32 prfs_index,
  training_params_t *training_params_ptr,
  uint8             *local_vars
);

/*===========================================================================

**  Function :  frwk_execute_training

** ==========================================================================
*/
/*!
* 
* @brief
*    Given an array of procedures, determine dependencies for the procedure,
* execute them (if not yet completed) and then execute the procedure.
* 
* @param[in] ddr            Pointer to the sns shared data
* @param[in] proc           Pointer to the function pointer table 
* @param[in] array_size     Size of the function pointer table array
* @param[in] struct_index   Index of the training to execute
* @param[in] freq	    	    Training frequency
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
void frwk_execute_training
(
  BOOT_DDR_STRUCT *ddr,
  frwk_training_struct *proc,
  uint32 array_size,
  uint32 struct_index,
  uint32 freq,
  uint32 prfs_index,
  training_params_t *training_params_ptr,
  uint8             *local_vars
);

/*===========================================================================

**  Function :  training_requirement_check

** ==========================================================================
*/
/*!
* 
* @brief
*    Abort if a training marked as mandatory is not completed.
* 
* @param[in] ddr            Pointer to the sns shared data
* @param[in] proc           Pointer to the function pointer table
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
void training_requirement_check 
(
  BOOT_DDR_STRUCT *ddr,
  frwk_training_struct *proc
);

#endif  /* FRWK_TRAINING_H */

