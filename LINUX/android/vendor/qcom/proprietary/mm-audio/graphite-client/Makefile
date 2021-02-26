ALL=libgcs

all: $(ALL)

CC=$(CROSS)gcc

ifndef LDO
LDO=$(CC)
endif

ifndef CFLAGS
CFLAGS = -MMD -O2 -g
endif

CFLAGS += -I.
CFLAGS += -I./osal/inc
CFLAGS += -I./ipc/inc
CFLAGS += -I./cal-wrapper/inc
CFLAGS += -I./gcs/inc
CFLAGS += -fPIC -DPIC
CFLAGS += -D__unused=__attribute__\(\(__unused__\)\)
CFLAGS += -D_OPEN_WRT_
CFLAGS += -include stdint.h -include pthread.h
CFLAGS += -DUINT_MAX=\(~0U\)
LDFLAGS += -shared

LIBS += -lpthread

LIBGCS = libgcs.so
libgcs.so: $(OBJS)
	$(LDO) $(LDFLAGS) $(OBJS)  -o $(LIBGCS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(LIBS) $<

OBJS += ./osal/src/osal_cond.o
OBJS += ./osal/src/osal_dev.o
OBJS += ./osal/src/osal_lock.o
OBJS += ./osal/src/osal_mem.o
OBJS += ./osal/src/osal_thread.o

OBJS += ./ipc/src/ipc.o
OBJS += ./ipc/src/packetizer.o
OBJS += ./ipc/src/platform_info.o
OBJS += ./cal-wrapper/src/gcs_cal_ipq.o

OBJS += ./gcs/src/gcs.o

libgcs: $(OBJS) $(LIBGCS)

clean:
	rm -rf *.o *.a *.so *.d
	rm -rf osal/src/*.o osal/src/*.d
	rm -rf ipc/src/*.o ipc/src/*.d
	rm -rf cal-wrapper/src/*.o cal-wrapper/src/*.d
	rm -rf gcs/src/*.o gcs/src/*.d
