#ifndef TIMER_SLAVES_H

#define TIMER_SLAVES_H



/*=============================================================================

  Copyright (c) 2003-2013 QUALCOMM Technologies Incorporated.

  All rights reserved.

  Qualcomm Confidential and Proprietary

===========================================================================*/



/**

@file timer_slaves.h

@brief

Implements timer slave tasks



Traditionally, timer task has been used to call the calllback 

functions associated with the timer. This has often led to 

abusive callback holding timer task at ransom, affecting timer 

accuracy. By implementing these slave tasks, the callbacks can 

be offloaded to these slave tasks, and hence timer task is free 

to services other expiring timers. 



In this file, there is support for three slave tasks, which can 

be enabled by defining FEATURE_TIMER_SLAVE1, 

FEATURE_TIMER_SLAVE2 and FEATURE_TIMER_SLAVE3 respectively. 

These defines can be made in <product>.builds file.



INITIALIZATION AND SEQUENCING REQUIREMENTS



timer_slave_cmd_queue_init() should be called before timer_init()



The slave tasks should be started before the timer task in tmc. 

The reason is that there might be expired timers needed to be serviced

when timer task starts. This would require that timer slave task(s) 

are already started to process these timer callbacks.** 

*

*/



/*=============================================================================



                        EDIT HISTORY FOR MODULE



This section contains comments describing changes made to the module.

Notice that changes are listed in reverse chronological order.



$Header: //components/rel/core.qdsp6/2.1/services/time/src/timer_slaves.h#1 $



when       who     what, where, why

--------   ---     ------------------------------------------------------------

04/12/11   gparikh File created.



=============================================================================*/





/*include files*/

#include "comdef.h"

#include "timer.h"

#include "queue.h"
#if defined (FEATURE_DOG)
#include "dog.h"
#endif
/*include files end*/



/** Defines*/

#define TIMER_SLAVE_CMD_Q_SIG      0x0001

#define TIMER_SLAVE_RPT_TIMER_SIG  0x0002

#define MAX_TIMER_SLAVE_CMD_ELEMENTS 25





/*error checking*/

/*if slave task3 is defined, we make sure, that slave task2 is defined*/

#ifdef FEATURE_TIMER_SLAVE3
#ifndef FEATURE_TIMER_SLAVE2
#define FEATURE_TIMER_SLAVE2
#endif
#endif /*FEATURE_TIMER_SLAVE3*/



/*if slave task2 is defined, we make sure, that slave tasks1 is defined*/

#ifdef FEATURE_TIMER_SLAVE2
#ifndef FEATURE_TIMER_SLAVE1
#define FEATURE_TIMER_SLAVE1
#endif
#endif /*FEATURE_TIMER_SLAVE2*/

/*end error checking*/



/*this is to allocate memory*/

#ifdef FEATURE_TIMER_SLAVE3

#define MAX_SLAVE_TASKS              3

#elif defined FEATURE_TIMER_SLAVE2 

#define MAX_SLAVE_TASKS              2

#elif defined FEATURE_TIMER_SLAVE1  

#define MAX_SLAVE_TASKS              1

#else

#define MAX_SLAVE_TASKS              1

#endif

/*end defines*/





/*static variables go here*/

#ifdef DOG_DYNAMIC_API

/*  Dog variables - now we MAY be dynamic       */

/** @name Dog variables 

@{ */

extern uint32  dog_timer_slaves_rpt[MAX_SLAVE_TASKS]; /** Dog report ID. */

extern uint32  dog_timer_slaves_rpt_time[MAX_SLAVE_TASKS]; /** Report period. */

 

/** @} */ /* end_name Dog variables */

#endif /* DOG_DYNAMIC_API */

/*end static variable*/



/** @brief timer_slave_task_enum_type.*/

typedef enum

{

  TIMER_SLAVE_TASK1 = 0x0,

  TIMER_SLAVE_TASK2,

  TIMER_SLAVE_TASK3

}timer_slave_task_enum_type;



/**@brief timer_slave_operation_status_type.*/

typedef enum

{

  TIMER_SLAVE_RESULT_FAIL = 0x00,

  TIMER_SLAVE_RESULT_PASS = 0x01,

  TIMER_SLAVE_RESULT_UNKNOWN = 0xFF

}timer_slave_operation_status_type;



/**@brief timer_slave_cb_type.*/

typedef enum

{

  TIMER_SLAVE_SET_SIG,

  TIMER_SLAVE_CB_TYPE1,

  TIMER_SLAVE_CB_TYPE2

}timer_slave_cb_type;

/*end enum*/





/**@brief timer_slave_operations_type.*/

typedef struct

{

  timer_ptr_type                  timer;

  time_osal_notify_obj_ptr       timer_sigs_obj;

  time_osal_notify_data             sigs;

  timer_t1_cb_type                func_cb1;

  timer_t2_cb_type                func_cb2;

  int64                           run_time;

  timer_cb_data_type              data;  

}timer_slave_operations_type;





/**@brief timer_slave_cmd_type.*/

typedef struct

{

  q_link_type                        link;         /* Queue field */

  timer_slave_cb_type               call_back_type;       /*command id */

  timer_slave_operations_type       timer_slave_operation; /*timer operations */

}timer_slave_cmd_type;



/**@brief timer_slave_post_cmd_type.*/

typedef struct

{

  timer_ptr_type      timer_ext;

  timer_slave_cb_type cb_type;

  time_osal_notify_obj_ptr  timer_sigs_obj;

  time_osal_notify_data signal_to_be_set;

  timer_t1_cb_type cb1;

  timer_t2_cb_type cb2; 

  int64 run_time;

  timer_cb_data_type data;

  uint8 timer_slave_task_num;  

}timer_slave_post_cmd_params_type;

/*end struct defintions*/



/*function prototypes go here*/



/** 

@ingroup Timer Slaves Functions 

         

@brief 

timer slave task 1 entry point. This function is called from tmc.



@return

None.



@param[in]                                                               

 None  



@param[out]

 dword - Not used   

 

@dependencies  

Should be called before timer_task

 

@sideeffects 



*/

void timer_slave1_task 

(

  dword ignored

);





/** 

@ingroup Timer Slaves Functions 

         

@brief 

timer slave task 2 entry point. This function is called from tmc.



@return

None.



@param[in]                                                               

 dword - Not used  



@param[out]

 None   

 

@dependencies  

Should be called before timer_task

 

@sideeffects 



*/

void timer_slave2_task

(

  /* Unused parameter - required to match rex_task_func_type prototype */

  dword                           ignored    /*lint -esym(725,ignored) */

);





/** 

@ingroup Timer Slaves Functions 

         

@brief 

timer slave task 3 entry point. This function is called from tmc.



@return

None.



@param[in]                                                               

 dword - Not used  



@param[out]

 None   

 

@dependencies  

Should be called before timer_task

 

@sideeffects 



*/

void timer_slave3_task

(

  /* Unused parameter - required to match rex_task_func_type prototype */

  dword                           ignored    /*lint -esym(725,ignored) */

);



/** 

@ingroup Timer Slaves Functions 

         

@brief 

Initializes timer slave commmand queues



@return

None.



@param[in]                                                               

 None  



@param[out]

 None   

 

@dependencies  

Should be called before timer slave tasks are started

 

@sideeffects 



*/

void timer_slave_cmd_queue_init

(

  void

);



/** 

@ingroup Timer Slaves Functions 

         

@brief 

Allocates memory from the command buffer of the specified

slave task



@return

None.



@param[in]                                                               

timer_slave_task_enum_type - slave task from which the memory is 

                             to be obtained



@param[out]

timer_slave_cmd_type* - pointer to cmd buffer memory

 

@dependencies  

None

 

@sideeffects 



*/

timer_slave_cmd_type  *timer_slave_get_cmd_buf

( 

  timer_slave_task_enum_type time_slave_task_num 

);



/** 

@ingroup Timer Slaves Functions 

         

@brief 

Post commands on appropriate slave task



@return

None.



@param[in]                                                               

* timer_slave_post_cmd_params_type* - ptr to slave command

*                                     structure



@param[out]

timer_slave_operation_status_type - status of the operation

 

@dependencies  

None

 

@sideeffects 



*/

timer_slave_operation_status_type timer_slave_post_cmd

(  

  timer_slave_post_cmd_params_type* slave_cmd_ptr

);



/** 

@ingroup Timer Slaves Functions 

         

@brief 

Gets the priorities boundaries that decide the

assignment of slave task



@return

dword The cutoff priority for the bin specified



@param[in]                                                               

  timer_slave_task_enum_type - bin specified



@param[out]

None

 

@dependencies  

None

 

@sideeffects 



*/

dword timer_slave_get_priority_boundary

(

  timer_slave_task_enum_type bin

);



/** 

@ingroup Timer Slaves Functions 

         

@brief 

Registers the task with dog. Slave tasks when started, are not

registered with dog due to sequence dependency (dog task is started

after the slave tasks). 



@return

None



@param[in]                                                               

   timer_slave_task_enum_type - task to be registered with dog



@param[out]

None

 

@dependencies  

dog task needs to be started before calling this fn

 

@sideeffects 



*/

void timer_slave_register_dog

(

  timer_slave_task_enum_type timer_slave_num

);



/** 

@ingroup Timer Slaves Functions 

         

@brief 

Assigns slave task to the timer during timer_def



@return

None



@param[in]                                                               

   timer_ptr_type - pointer to timer that needs to be

                assigned a slave



@param[out]

None

 

@dependencies  

FEATURE_TIMER_SLAVE1 should be defined

 

@sideeffects 



*/

uint32 timer_assign_slave_task_to_timer
(
  void
);


/** 

@ingroup Timer Slaves Functions

@brief 

Initializes slave tasks 

@return

None

@param[in]                                                               
None

@param[out]
None

@dependencies  

FEATURE_TIMER_SLAVE<X> should be defined to
start the timer_slave<x> task

@sideeffects 
Should be started before timer task
*/

void timer_slave_tasks_init
(
  void
);


#endif /*TIMER_SLAVE_TASK1*/

