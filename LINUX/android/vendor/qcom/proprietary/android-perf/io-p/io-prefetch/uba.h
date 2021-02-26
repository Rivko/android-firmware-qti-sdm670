/******************************************************************************
  @file    uba.h
  @brief   nheader file to maintain common datastructures

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "dblayer.h"

typedef struct palm_data {
    char pkg_name[PKG_NAME_LEN];
    bool empty;
    bool cached;
    int memory_rss;
    long last_empty_time;
    bool do_not_launch;
    bool hit;
    bool comp;
} palm_table;

extern palm_table *tracker;

int get_preferred_apps(char **out_list);
void update_palm_table(char *pkg_name, int kill, int launch);
void compute_time_day_slot(char *week_day, int *time_slot);
