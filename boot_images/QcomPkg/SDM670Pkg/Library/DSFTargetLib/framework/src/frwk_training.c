/*=============================================================================

                              ddrsns framework

GENERAL DESCRIPTION
  This file contains the main control functions for ddrsns framework

Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/09/16   tw     Initial creation
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "frwk_training.h"
#include "ddrss.h"
#include "ddr_external.h"
#include "ddr_log.h"
#include "HAL_SNS_DDR.h"
#include <string.h> 
#include "dcb.h"
//#include "hw_regs.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

static boolean training_complete[32] = {0};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

uint32 getArraySize (frwk_training_struct *proc)
{
  uint32 arraySize = 0;
  frwk_training_struct *cur_proc;
  
  for (cur_proc = proc; cur_proc->func_ptr != NULL; cur_proc++)
  {
    arraySize++;
  }
  return arraySize;
}

uint32 getStructIndex (frwk_training_struct *proc, uint32 array_size, training_func_ptr func)
{
  uint32 structIndex = 0xFF, i = 0;
  
  for (i = 0; i < array_size; i++)
  {
    if (proc[i].func_ptr == func)
    {
      structIndex = i;
      break;
    }
  }
  return structIndex;    
}
    

/*===========================================================================

**  Function :  frwk_do_trainings

** ==========================================================================
*/
/*!
* 
* @brief
*    given training lists execute them with clock switching requirements
* 
* @param[in] ddr            			Pointer to the sns shared data
* @param[in] pre_training_array         Pointer to the function pointer table 
* @param[in] do_training_array       Pointer to the function pointer table 
* @param[in] post_training_array        Pointer to the function pointer table 
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
  frwk_training_struct *pre_training_array,
  frwk_training_struct *do_training_array,
  frwk_training_struct *post_training_array,
  training_params_t    *training_params_ptr,
  uint8                *local_vars  
)
{
  uint32 i, array_size;
  uint8  train_stop_idx =0;
  uint8  train_start_idx =0;
 
  if(ddr == NULL)
  {
    ddr_abort();
  }
  else
  {

    uint32 prfs_index = 0;
    uint32 freq = 200000;
    uint8  training_freq_index;
    uint32  training_prfs_table[MAX_TRAINING_FREQ_NUM]={(NUM_PRFS_BANDS-1),(NUM_PRFS_BANDS-2),(NUM_PRFS_BANDS-3),(NUM_PRFS_BANDS-4),(NUM_PRFS_BANDS-5),(NUM_PRFS_BANDS-6),(NUM_PRFS_BANDS-7),(NUM_PRFS_BANDS-8)};
    uint32  training_freq_idx[MAX_TRAINING_FREQ_NUM]={0,1,2,3,4,5,6,7};
    /* Loop through pre_training array */
    array_size = getArraySize(pre_training_array);
    frwk_loop_training_seq(ddr, 
                           pre_training_array, 
                           array_size, 
                           freq, 
                           prfs_index, 
                           training_params_ptr,
                           local_vars);
	
    /* Loop through do_training array, for all frequencies identified based on band */
    array_size = getArraySize(do_training_array);

   

    for (training_freq_index = 0; training_freq_index < MAX_TRAINING_FREQ_NUM; training_freq_index++)
    {
      if (ddr->shrm_ddr_ptr->misc.training_freq_in_khz[training_freq_index] == ddr->shrm_ddr_ptr->ddr_runtime.max_ddr_frequency)
      {
       train_start_idx =  training_freq_idx[training_freq_index];    
      }
      if (ddr->shrm_ddr_ptr->misc.training_freq_in_khz[training_freq_index] == DSF_MIN_SUPPORTED_DDR_TRAINING_FREQ)
      {
       train_stop_idx =  training_freq_idx[training_freq_index];      
      }      
    }
    for(i = train_start_idx ; i <= train_stop_idx ; i++)
    {
      freq = ddr->shrm_ddr_ptr->misc.training_freq_in_khz[i];
      ddr_external_set_clk_speed(freq); 
      
      /* current prfs index*/
	    prfs_index = training_prfs_table[i];  
      
     // frwk_loop_training_seq(ddr, do_training_array, array_size, freq, prfs_index, training_params_ptr);
      frwk_loop_training_seq(ddr, 
                             do_training_array, 
                             array_size, 
                             freq, 
                             prfs_index, 
                             training_params_ptr,
                             local_vars);
    }
    training_requirement_check(ddr, do_training_array);
  
    /* Loop through post_training array */
    array_size = getArraySize(post_training_array);
    frwk_loop_training_seq(ddr, 
                           post_training_array, 
                           array_size, 
                           freq, 
                           prfs_index, 
                           training_params_ptr,
                           local_vars);     
  }
} /* frwk_do_trainings */

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
* @param[in] proc          Pointer to the function pointer table
* @param[in] array_size     Size of the function pointer table array
* @param[in] freq			      Training frequency
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
  //uint8 struct_index,
  uint32 freq,
  uint32 prfs_index,
  training_params_t *training_params_ptr,
  uint8             *local_vars
)
{
  frwk_training_struct *cur_proc;
  uint8 struct_index = 0;
  
  memset(training_complete, 0, sizeof(training_complete));

  if(ddr == NULL)
  {
    ddr_abort();
    return;
  }

  if (proc->func_ptr != NULL)
  {
    for ( cur_proc = proc; cur_proc->func_ptr != NULL; cur_proc++ )
    {
      struct_index = getStructIndex (proc, array_size, cur_proc->func_ptr);
      frwk_execute_training (ddr, 
                             proc, 
                             array_size, 
                             struct_index, 
                             freq, 
                             prfs_index, 
                             training_params_ptr,
                             local_vars);
    }
  }
} /* frwk_loop_training_seq */


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
)
{
  uint32 i, dep_struct_idx;

  if (!training_complete[proc[struct_index].index])
  {
    for (i = 0; i < MAX_DEPENDENCIES; i++)
    {      
      if (proc[struct_index].dependencies[i] != NULL)
      {
        dep_struct_idx = getStructIndex(proc, array_size, proc[struct_index].dependencies[i]);
        
        if (dep_struct_idx != 0xFF)
        {
          frwk_execute_training(ddr, 
                                proc, 
                                array_size, 
                                dep_struct_idx, 
                                freq, 
                                prfs_index, 
                                training_params_ptr,
                                local_vars);
        }
        else
        {
          ddr_abort();
        }
      }
      else
      {
        ddr_printf (DDR_NORMAL,"Executing %s\n", proc[struct_index].func_name);
        (proc[struct_index].func_ptr)(ddr, 
                                      ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask, 
                                      ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect, 
                                      freq, 
                                      prfs_index, 
                                      training_params_ptr,
                                      local_vars);
        training_complete[proc[struct_index].index] = 1;
        break;
      }
    }
  }

} /* frwk_execute_training */

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
void training_requirement_check (BOOT_DDR_STRUCT *ddr, frwk_training_struct *proc)
{
	frwk_training_struct *cur_proc;
  uint32 freq = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[ddr->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels-1].clk_freq_in_khz;
  
  for (cur_proc = proc; cur_proc->func_ptr != NULL; cur_proc++)
  {
    if (strncmp((cur_proc->func_name), "rcw", MAX_STRING_SIZE))
    {
      if ((freq >= 1017000) && (cur_proc->trng_req == MANDATORY) && !training_complete[cur_proc->index])
      {
        ddr_abort();
      }
    }
    else if ((cur_proc->trng_req == MANDATORY) && !training_complete[cur_proc->index])
    {
      ddr_abort();
    }
  }
} /* training_requirement_check */
