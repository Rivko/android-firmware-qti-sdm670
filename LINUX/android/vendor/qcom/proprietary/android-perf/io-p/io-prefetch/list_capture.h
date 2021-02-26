/******************************************************************************
  @file    list_capture.c
  @brief   header file to capture list of files

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/


#include "maintain_db.h"
#include "dblayer.h"

/*Public API*/
int start_capture(pid_t pid, char *pkg_name, char *code_path, bool ofr);
int stop_capture();
