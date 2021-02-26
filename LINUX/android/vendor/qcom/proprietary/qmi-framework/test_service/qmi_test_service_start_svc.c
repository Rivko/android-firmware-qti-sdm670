/******************************************************************************
  -----------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <getopt.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"

extern void *qmi_test_service_register_service(qmi_csi_os_params *os_params, uint32_t serv_instce);

int instance_connect = 0;
static void usage(const char *fname)
{
  printf("\n **********************************************\n");
  printf("Usage: %s -e <val>"
      "\nPARAMS can be:\n"
      " -e <value>, Specific Instance of Service to start"
      "\n", fname);
  printf("- If no instance ID provided, test ping service is started with instance ID as 0\n");
  printf("\n **********************************************\n");
}
static void parse_command(int argc, char *const argv[])
{
  int command;
  struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"instance_connect", required_argument, NULL, 'e'},
    {NULL, 0, NULL, 0},
  };
  while ((command = getopt_long(argc, argv, "h:e:", longopts, NULL)) != -1)
  {
    switch (command)
    {
      case 'h':
        usage(argv[0]);
        exit(0);
      case 'e':
        instance_connect = atoi(optarg);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

/*=============================================================================
  FUNCTION qmi_start_ping_service
=============================================================================*/
void qmi_test_service_start_service(uint32_t serv_inst)
{
  qmi_csi_os_params os_params,os_params_in;
  fd_set fds;
  char buf[10];
  void *sp;

  sp = qmi_test_service_register_service(&os_params, serv_inst);

  if(!sp)
  {
    printf("Unable to register service!\n");
    exit(1);
  }

  /* This loop calls a blocking select to read from the UDP port the off target
     test is using */
  while(1)
  {
    fds = os_params.fds;
    FD_SET(STDIN_FILENO, &fds);
    select(os_params.max_fd+1, &fds, NULL, NULL, NULL);
    /* Test for user input of the ctrl+d character to terminate the server */
    if(FD_ISSET(STDIN_FILENO, &fds))
    {
      if(read(STDIN_FILENO, buf, sizeof(buf)) <= 0)
      {
        break;
      }
    }
    os_params_in.fds = fds;
    qmi_csi_handle_event(sp, &os_params_in);
  }
  qmi_csi_unregister(sp);
  printf("Server Terminated....\n");
}

/*=============================================================================
  FUNCTION main
=============================================================================*/
int main(int argc, char *argv[])
{
  parse_command(argc, argv);
  printf("\n\n\nQMI TEST PING SERVICE STARTED...\n");
  printf("Test parameters\n");
  printf("   Instance ID  : %d\n",(unsigned int) instance_connect);
  printf("TO STOP SERVER: Press Ctrl+d ....\n");

  /* The code in start_ping_service is the on-target intiialization code */
  qmi_test_service_start_service(instance_connect);

  return 0;
}
