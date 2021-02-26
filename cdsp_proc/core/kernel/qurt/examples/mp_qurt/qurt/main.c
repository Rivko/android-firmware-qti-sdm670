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
#include <fcntl.h>
#include <assert.h>
#include <qurt.h>
#include <qurt_timer.h>

int main(int argc, char **argv)
{
    int ret, status, cid1, cid2, pid;
    volatile int count = 0;

    printf("qurt pid is %d\n", qurt_getpid());

    //currently qurt_spawn starts all child processes
    cid1 = qurt_spawn("app1.pbn");
    cid2 = qurt_spawn("app2.pbn");

    printf("Process app1.pbn launched with ID=%d, app2.pbn launched with ID=%d\n", cid1, cid2);

    while (count++ < 100000);
    
#if 0     // qurt_space_switch() API no longer supported
    //test fast space switch
    pid = qurt_space_switch(cid1);
    printf("after space switch to 1, current pid is %d\n", qurt_getpid());
    
    qurt_space_switch(cid2);
    printf("after space switch to 2, current pid is %d\n", qurt_getpid());

    //switch back to original guest OS process ID
    qurt_space_switch(pid);
#endif

    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);
    
    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);

    printf( "qurt main exit.\n");
    return 0;
}

