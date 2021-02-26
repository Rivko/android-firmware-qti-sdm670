/*=============================================================================

                                    debug_me.c

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <qurt.h>

static qurt_anysignal_t my_signal;
extern int calculator(int x, int y, char op);
static char dbgr_msg[64];
extern int allow_err_fatal;

static qurt_mutex_t nstop_mutex;
static int iter_num;
static qurt_anysignal_t nstop_signal;
void debug_non_stop();

void my_main(void *param)
{
    int a =5, b = 3, x, y, z;

    qurt_anysignal_init(&my_signal);
    while(1)
    {
        x = calculator(a, b, '+');
        y = calculator(a, b, '-');
        z = calculator(a, b, '*');
        snprintf(dbgr_msg, sizeof(dbgr_msg), "(%d+%d=%d) (%d-%d=%d) (%d*%d=%d)", a, b, x, a, b, y, a, b, z);
        printf("%s\n", dbgr_msg);
        //if (allow_err_fatal)
        //{
        //    ASSERT(!"Deliberate Failure Message Inserted");
        //}
        qurt_anysignal_wait(&my_signal, 1);
        qurt_anysignal_clear(&my_signal, 1);
    }
    qurt_anysignal_destroy(&my_signal);
}

void debug_me_continue(void)
{
    qurt_thread_attr_t attr;
    qurt_thread_t thread_id;
    void *stack_ptr;    
    
    debug_non_stop();
    
    qurt_thread_attr_init(&attr);
    qurt_thread_attr_set_name(&attr, "DEBUG_ME");
    qurt_thread_attr_set_priority(&attr, (qurt_thread_get_priority(qurt_thread_get_id()) + 1));
    stack_ptr = malloc(0x1000);
    assert(NULL != stack_ptr);
    qurt_thread_attr_set_stack_addr(&attr, stack_ptr);
    qurt_thread_attr_set_stack_size(&attr, 0x1000);
    assert(0 == qurt_thread_create(&thread_id, &attr, my_main, 0));
    
    
}

void print_me(char *name, int print_num)
{
    printf("**************************************************\n");
    printf("Name: %s \t Iteration: %d\n", name, print_num);
    printf("**************************************************\n");
}


void nstop_main(void *param)
{
	int iteration_number = 0;
    qurt_mutex_lock(&nstop_mutex);
	//ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	//__asm__ __volatile__ ("trap0(#0xDB)\n");
    iter_num++;
	iteration_number = iter_num;
	qurt_mutex_unlock(&nstop_mutex);
	
	while(iteration_number < 200)
	{
		iteration_number +=10;
		print_me(param, iteration_number);  
    }
}

void debug_non_stop()
{
    #define NUM_THREADS 5
    qurt_thread_attr_t attr[NUM_THREADS];
    qurt_thread_t thread_id[NUM_THREADS];
    int *stack[NUM_THREADS];
    char names[NUM_THREADS][12];
    
    int i;
	printf("In debug_non_stop\n");
    
    qurt_anysignal_init(&nstop_signal);
    qurt_mutex_init(&nstop_mutex);
    // Create 5 threads and make them all call the same fn. 
    // Set bkpt in that function & test non stop mode
    for (i = 0; i < NUM_THREADS; i++)
    {
        qurt_thread_attr_init(&attr[i]);
        snprintf(names[i], 12, "NONSTOP_T_%d_\0", i);
        qurt_thread_attr_set_name(&attr[i], names[i]);
        qurt_thread_attr_set_priority(&attr[i], (qurt_thread_get_priority(qurt_thread_get_id()) + 1));
        stack[i] = malloc(0x1000);
        assert(NULL != stack[i]);
        qurt_thread_attr_set_stack_addr(&attr[i], stack[i]);
        qurt_thread_attr_set_stack_size(&attr[i], 0x1000);
        assert(0 == qurt_thread_create(&thread_id[i], &attr[i], nstop_main, (void *)names[i]));   
    }
    
    while(1)
    {
        qurt_anysignal_wait(&nstop_signal, 1); 
        qurt_anysignal_clear(&nstop_signal, 1);
    }
    qurt_mutex_destroy(&nstop_mutex);    

}

