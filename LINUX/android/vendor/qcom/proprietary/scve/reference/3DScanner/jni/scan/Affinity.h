/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AFFINITY_H_
#define AFFINITY_H_

#include <sys/syscall.h>
#include <unistd.h>

#define __NCPUBITS  (8 * sizeof (unsigned long))

#define CPU_SET(cpu, cpusetp) \
  ((cpusetp)->__bits[(cpu)/__NCPUBITS] |= (1UL << ((cpu) % __NCPUBITS)))
#define CPU_ZERO(cpusetp) \
  memset((cpusetp), 0, sizeof(cpu_set_t))


void setCurrentThreadAffinityMask(cpu_set_t &mask, int & syscallres)
{
    int err;
    pid_t pid = gettid();
    syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);

    //if (syscallres)
    //{
     //   err = errno;
      //  LOG("Error in the syscall setaffinity: mask=%d=0x%x err=%d=0x%x", mask, mask, err, err);
    //}
}


#endif /* AFFINITY_H_ */
