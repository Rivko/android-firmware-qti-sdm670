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
#include <qurt_event.h>

#include <string.h>
#include <qurt_qdi_driver.h>

struct ringbuf {
  char * start;
  char * volatile head;
  char * volatile tail;
  char * end;
  qurt_mutex_t rmutex;
  qurt_mutex_t wmutex;
};

struct devpipe {
  qurt_qdi_obj_t qdiobj;
  struct ringbuf *readring;
  struct ringbuf *writering;
};

static int ringbuf_read(int client_handle,
                        struct ringbuf *ring,
                        void *buf,
                        int len)
{
  int orig_len;
  char *tail;
  char *head;
  int err;
  int cnt;

  orig_len = len;

  qurt_mutex_lock(&ring->rmutex);
  tail = ring->tail;
  while ((head = ring->head) == tail)
    qurt_futex_wait((void *)&ring->head, (int)head);
  for (;;) {
    if (head >= tail)
      cnt = head-tail;
    else
      cnt = ring->end-tail;
    if (cnt > len)
      cnt = len;
    if (cnt == 0)
      break;
    // printf("Reading %d at %X (%d)\n", cnt, (unsigned)tail, len);
    err = qurt_qdi_copy_to_user(client_handle, buf, tail, cnt);
    if (err < 0)
       return err;
    buf = (char *)buf + cnt;
    tail = (char *)tail + cnt;
    len -= cnt;
    if (tail == ring->end)
      tail = ring->start;
    ring->tail = tail;
    qurt_futex_wake((void *)&ring->tail, 1);
  }
  qurt_mutex_unlock(&ring->rmutex);

  return orig_len-len;
}

static int ringbuf_write(int client_handle,
                         struct ringbuf *ring,
                         const void *buf,
                         int len)
{
  char *head_plus_one;
  int orig_len;
  char *tail;
  char *head;
  int err;
  int cnt;

  orig_len = len;

  qurt_mutex_lock(&ring->wmutex);
  for (;;) {
    head = ring->head;
    head_plus_one = head+1;
    if (head_plus_one == ring->end)
      head_plus_one = ring->start;
    while ((tail = ring->tail) == head_plus_one)
      qurt_futex_wait((void *)&ring->tail, (int)tail);
    if (tail > head)
      cnt = tail-head_plus_one;
    else if (tail == ring->start)
      cnt = ring->end-head_plus_one;
    else
      cnt = ring->end-head;
    if (cnt > len)
      cnt = len;
    if (cnt == 0)
      break;
    // printf("Writing %d at %X (%d)\n", cnt, (unsigned)head, len);
    err = qurt_qdi_copy_from_user(client_handle, head, buf, cnt);
    if (err < 0)
       return err;
    buf = (char *)buf + cnt;
    head = (char *)head + cnt;
    len -= cnt;
    if (head == ring->end)
      head = ring->start;
    ring->head = head;
    qurt_futex_wake((void *)&ring->head, 1);
  }
  qurt_mutex_unlock(&ring->wmutex);

  return orig_len-len;
}

int devpipe_read(int client_handle,
                 struct devpipe *ppipe,
                 void *buf,
                 int buflen)
{
   // printf("devpipe_read %p %d\n", buf, buflen);
   return ringbuf_read(client_handle, ppipe->readring, buf, buflen);
}

int devpipe_write(int client_handle,
                  struct devpipe *ppipe,
                  const void *buf,
                  int buflen)
{
   // printf("devpipe_write %p %d\n", buf, buflen);
   return ringbuf_write(client_handle, ppipe->writering, buf, buflen);
}

static char ring0buf[16384];
static char ring1buf[16384];

static struct ringbuf ring0 = {
  ring0buf, ring0buf, ring0buf, ring0buf+sizeof(ring0buf)
};

static struct ringbuf ring1 = {
  ring1buf, ring1buf, ring1buf, ring1buf+sizeof(ring1buf)
};

struct devpipe_opener {
   qurt_qdi_obj_t qdiobj;
};

static int devpipe_invoke(QDI_INVOKE_ARGS);

static void devpipe_release(qurt_qdi_obj_t *pobj)
{
   free(pobj);
}

static int devpipe_open(int client_handle,
                        struct devpipe_opener *objptr,
                        const char *devname,
                        int mode)
{
   struct devpipe *p_devpipe;
   int pipenum;

   pipenum = -1;
   if (!strcmp(devname, "/dev/kpipe0"))
      pipenum = 0;
   if (!strcmp(devname, "/dev/kpipe1"))
      pipenum = 1;

   if (pipenum == -1)
      return -1;

   p_devpipe = malloc(sizeof(*p_devpipe));
   if (p_devpipe == NULL)
      return -1;

   p_devpipe->qdiobj.invoke = devpipe_invoke;
   p_devpipe->qdiobj.refcnt = QDI_REFCNT_INIT;
   p_devpipe->qdiobj.release = devpipe_release;
   if (pipenum == 0) {
      p_devpipe->readring = &ring0;
      p_devpipe->writering = &ring1;
   } else {
      p_devpipe->readring = &ring1;
      p_devpipe->writering = &ring0;
   }

   return qurt_qdi_new_handle_from_obj_t(client_handle, &p_devpipe->qdiobj);
}

static int devpipe_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
   struct devpipe *me;

   me = (void *)pobj;

   switch (method) {
   case QDI_OPEN:
      return devpipe_open(client_handle, (void *)pobj, a1.ptr, a2.num);
   case QDI_READ:
	  if (a3.num < 0)
	      return -1;
      return devpipe_read(client_handle, me, a2.ptr, a3.num);
   case QDI_WRITE:
	  if (a3.num < 0)
		  return -1;
      return devpipe_write(client_handle, me, a2.ptr, a3.num);
   case QDI_PRIVATE:
      printf("9 arguments %d %d %d %d %d %d %d %d %d\n",
             a1.num, a2.num, a3.num,
             a4.num, a5.num, a6.num,
             a7.num, a8.num, a9.num);
      return 0;
   default:
      return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
   return -1;
}

static void devpipe_register(void)
{
   struct devpipe_opener *p_opener;

   qurt_mutex_init(&ring0.rmutex);
   qurt_mutex_init(&ring0.wmutex);
   qurt_mutex_init(&ring1.rmutex);
   qurt_mutex_init(&ring1.wmutex);

   p_opener = malloc(sizeof(*p_opener));
   if (p_opener == NULL)
      return;

   p_opener->qdiobj.invoke = devpipe_invoke;
   p_opener->qdiobj.refcnt = QDI_REFCNT_PERM;
   p_opener->qdiobj.release = devpipe_release;
   qurt_qdi_register_devname("/dev/kpipe0", p_opener);
   qurt_qdi_register_devname("/dev/kpipe1", p_opener);
}

void __qdsp6_bsp_init(void)
{
    __asm__ __volatile__ (
        "r11 = r11 \n"
        "r11 = #0x1234\n"
        "jumpr r31  \n"
        :::"r11");
}

int main(int argc, char **argv)
{
  int ret, status;

  printf("qurt pid is %d\n", qurt_getpid());

  {
     qurt_sysenv_procname_t pn;
     qurt_sysenv_get_process_name(&pn);
     printf("Running in ASID %u, name '%s'\n",
            pn.asid, pn.name);
  }

  devpipe_register();

  qurt_spawn("app1.pbn");
  qurt_spawn("app2.pbn");

    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);
    
    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);

    
  printf("qurt main exit.\n");

  return 0;
}

