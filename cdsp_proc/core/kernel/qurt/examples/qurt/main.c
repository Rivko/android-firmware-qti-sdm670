/*=============================================================================

                                    main.c

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qurt.h>
#define MAX_THREADS 6 

int ceiling_prio = (255-200);
static qurt_sem_t to_start; 
static int t[MAX_THREADS];
int original_prio[MAX_THREADS];

void __hexagon_bsp_init(void)
{
    __asm__ __volatile__ (
        "r11 = r11 \n"
        "r11 = #0x1234\n"
        "jumpr r31  \n"
        :::"r11");
}

void ThreadMain(void * name)
{
    static qurt_barrier_t memtest_barrier;

    static qurt_sem_t mutex_sem;

	  static qurt_pipe_t pipe;
	  static qurt_pipe_data_t pipedata[5];
    static qurt_pipe_attr_t pipe_attr;
    int id = ((int)name)<<24>>24;
    
    int k;
    if( id == 0) {
        qurt_barrier_init( &memtest_barrier, 3 );

        qurt_sem_init_val(&mutex_sem,0);

        qurt_pipe_attr_set_buffer(&pipe_attr, pipedata);
        qurt_pipe_attr_set_elements(&pipe_attr, 5);
		    qurt_pipe_init(&pipe, &pipe_attr);

        qurt_sem_add(&to_start, MAX_THREADS);

    }

    qurt_sem_down(&to_start);

    printf( "thread %d woken\n", id);

    static int mutex_threads = 20;

#define MAX_PIPE_LOOP 10000

	int pipe_loop;
	qurt_pipe_data_t data;
    //qurt_prio_set
    if( id < 50 && id >= 30){
		if(id%2 == 0){
			for( pipe_loop = 0 ; pipe_loop < MAX_PIPE_LOOP; pipe_loop++ ){
				if((pipe_loop % 1000) == 0){
					printf("thread %d sent %d msgs\n", id, pipe_loop);
				}
				qurt_pipe_send(&pipe, ((unsigned long long)id)<<32|pipe_loop );
			}
		}
		else{
			for( pipe_loop = 0 ; pipe_loop < MAX_PIPE_LOOP; pipe_loop++ ){
				if((pipe_loop % 1000) == 0){
					printf("thread %d received %d msgs\n", id, pipe_loop);
				}
				data = qurt_pipe_receive(&pipe);
			}
		}
    }

    qurt_thread_exit((int)name);
}

int thread_create(void *pc, void *stack, unsigned int stack_size, char *arg, int prio){
    
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    int ret;
    char tname[16];
    int id = ((int)arg)<<24>>24;

    snprintf(tname, "T%d",id);

    printf( "thread %d started\n", id );
    
    qurt_thread_attr_init (&attr);
    qurt_thread_attr_set_name(&attr, (char*)tname);
    qurt_thread_attr_set_stack_size (&attr, stack_size);
    qurt_thread_attr_set_stack_addr (&attr, stack);
    qurt_thread_attr_set_priority (&attr, (unsigned short)prio);
    /* TIMETEST IDs 0 - 31 are reserved for interrrupts */
    qurt_thread_attr_set_timetest_id (&attr, id + 31);

    ret = qurt_thread_create(&tid, &attr, pc, (void *)arg);
    if (ret == -1) {
        printf(" failed to create thread \n");
        assert (0);
    }
    return tid;
}


void test(void *arg){
    int myid = (int) arg;
    char names[MAX_THREADS][20];
    void *stack_start[MAX_THREADS];
    void *stack_top[MAX_THREADS];
    int i;
    int j;
    for( j = 0; j < 2; j++ ){
        qurt_sem_init_val(&to_start,0);
        for( i = 0; i < MAX_THREADS; i++ ){
            int prio = 100-i;
            snprintf( names[i], 20, "t%d", i );
            stack_start[i] = malloc(4096);
            
            t[i] = thread_create(ThreadMain, stack_start[i], 4096, (void *)(myid<<16|j<<8|i), /*unsigned int prio*/prio); 
            original_prio[i] = prio;
        }
        int r;
        int status; 
        for( i = 0; i < MAX_THREADS; i++ ){
            r = qurt_thread_join(t[i], &status ); 
            free(stack_start[i]);
            printf( "%d rount %d t%d exit return %p status %p\n", myid, j, i, r, status);
            if( r == 0 ){
                assert( status == ((myid<<16|j<<8|i)));
            }
        }
        qurt_sem_destroy(&to_start);
    }
    qurt_thread_exit(0);
} 

int main(int argc, char **argv)
{

    int t1, t2;
    void * stack = malloc( 4096 );

    t1 = thread_create(test, stack, 4096, (void *)0x0, /*unsigned int prio*/0); 

    int r;
    int status; 
    r = qurt_thread_join(t1, &status ); 
    printf( "test1 exit return %p status %p\n", r, status);
    return 0;
}

