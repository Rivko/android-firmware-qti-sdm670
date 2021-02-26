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
#include <string.h>

#include "qurt_qdi.h"
#include "qurt_event.h"
#include "qurt_mutex.h"
#include "qurt_thread.h"

#define STKSIZE  4096

struct threadinfo {
  volatile int finished;
  const char *message;
  int role;
  qurt_thread_attr_t attr;
  qurt_thread_t tid;
#define PRODUCER 0
#define CONSUMER 1
};

void threadmain(void *arg)
{
  struct threadinfo *ti;
  static char wbuf[1024];
  static char rbuf[1024];
  char ch;
  int h;
  int i;

  ti = arg;

  printf("%s\n", ti->message);

  if (ti->role == PRODUCER) {
    /* Push 512K of data into the pipe, 1K at a time */
    h = qurt_qdi_open("/dev/kpipe0", 2);
    printf("pipe0 == %d\n", h);
    for (i=0; i<512; i++) {
      qurt_qdi_write(h, wbuf, sizeof(wbuf));
    }
    printf("Finished writing\n");
    /* Wait for 1 byte from the other side, which should
       indicate that he's done reading it */
    qurt_qdi_read(h, &ch, 1);
  } else {
    /* Read 512K of data from the pipe, 1K at a time */
    h = qurt_qdi_open("/dev/kpipe0", 2);
    printf("pipe1 == %d\n", h);
    for (i=512*1024; i; ) {
      i -= qurt_qdi_read(h, rbuf, i > sizeof(rbuf) ? sizeof(rbuf) : i);
    }
    printf("Finished reading\n");
    qurt_qdi_write(h, "$", 1);
  }

  qurt_qdi_handle_invoke(h, QDI_PRIVATE, 11, 22, 33, 44, 55, 66, 77, 88, 99);

  qurt_qdi_close(h);

  ti->finished = 1;
  qurt_futex_wake((void *)&ti->finished, 100);
  qurt_thread_stop();
}

int main(int argc, char **argv)
{
  struct threadinfo ti[2];
  unsigned prio;

  printf("app1 main started\n");

  {
     qurt_sysenv_procname_t pn;
     qurt_sysenv_get_process_name(&pn);
     printf("Running in ASID %u, name '%s'\n",
            pn.asid, pn.name);
  }

  ti[0].message = "producer started";
  ti[0].finished = 0;
  ti[0].role = PRODUCER;

  prio = qurt_thread_get_priority(qurt_thread_get_id());

  qurt_thread_attr_init(&ti[0].attr);
  qurt_thread_attr_set_stack_size(&ti[0].attr, STKSIZE);
  qurt_thread_attr_set_stack_addr(&ti[0].attr, malloc(STKSIZE));
  qurt_thread_attr_set_priority(&ti[0].attr, prio);
  qurt_thread_create(&ti[0].tid, &ti[0].attr, threadmain, &ti[0]);

  while (ti[0].finished == 0)
    qurt_futex_wait((void *)&ti[0].finished, 0);

  printf("app1 first test thread done\n");

  ti[0].message = "consumer started";
  ti[0].finished = 0;
  ti[0].role = CONSUMER;

  prio = qurt_thread_get_priority(qurt_thread_get_id());

  qurt_thread_attr_init(&ti[0].attr);
  qurt_thread_attr_set_stack_size(&ti[0].attr, STKSIZE);
  qurt_thread_attr_set_stack_addr(&ti[0].attr, malloc(STKSIZE));
  qurt_thread_attr_set_priority(&ti[0].attr, prio);
  qurt_thread_create(&ti[0].tid, &ti[0].attr, threadmain, &ti[0]);

  while (ti[0].finished == 0)
    qurt_futex_wait((void *)&ti[0].finished, 0);

  printf("app1 second test thread done\n");

  qurt_thread_stop();

  return 0;
}

