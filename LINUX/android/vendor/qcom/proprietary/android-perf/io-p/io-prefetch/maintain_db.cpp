/******************************************************************************
  @file    maintain_db.c
  @brief   Source file to maintain database of files

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <cutils/properties.h>
#include <pthread.h>
#include "maintain_db.h"
#include "uba.h"

static unsigned int pkg_retention_threshold = 28800 * 60;
static unsigned int pkg_cleanup_interval = 2880 * 60;
static int last_launch_threshold = 21600 * 60;
static pthread_t db_cleanup_thread;

/* return current time in milliseconds */
static double now_secs(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (double)1.0 * res.tv_sec + (double) res.tv_nsec / 1e9;
}

static int ux_engine_db_cleanup(long cur_time)
{
    pkg_ux_details *cur_list = NULL;
    char *week_day = NULL;
    int time_slot;
    int total_pkgs = 0, num_pkgs = 0, lcount_avg = 0,index;
    int ret = 0;

    week_day = (char *) malloc(6*sizeof(char));
    if (week_day == NULL) {
        //Malloc failed. Most-probably low on memory.
        goto out;
    }

    compute_time_day_slot(week_day, &time_slot);
    total_pkgs = get_total_ux_pkgs(0);

    if (total_pkgs > 0)
        cur_list = (pkg_ux_details *) malloc(total_pkgs * sizeof(pkg_ux_details));
    else {
        ret = -1;
        goto out;
    }

    if(cur_list == NULL)
    {
        ret = -1;
        goto out;
    }

    num_pkgs = get_ux_pkg_list(cur_list, total_pkgs);

    if(num_pkgs == 0)
    {
        ret = -1;
        goto out;
    }

    for (index = 0; index < num_pkgs ; index++)
    {
        int factor = 0;
        if(cur_time - cur_list[index].last_time_launched >= last_launch_threshold) {
            factor = 4;
        } else {
            continue;
        }
        cur_list[index].timeslot_count_1 = cur_list[index].timeslot_count_1/factor;
        cur_list[index].timeslot_count_2 = cur_list[index].timeslot_count_2/factor;
        cur_list[index].timeslot_count_3 = cur_list[index].timeslot_count_3/factor;
        cur_list[index].timeslot_count_4 = cur_list[index].timeslot_count_4/factor;
        cur_list[index].num_launches = cur_list[index].timeslot_count_1 +
                                       cur_list[index].timeslot_count_2 +
                                       cur_list[index].timeslot_count_3 +
                                       cur_list[index].timeslot_count_4;
        cur_list[index].num_sub_launches = cur_list[index].num_sub_launches/factor;

        if(cur_list[index].num_launches > 0)
        {
            reset_ux_pkg_details(cur_list[index]);
        } else {
            remove_ux_pkg(cur_list[index].pkg_name, week_day);
        }
    }
    ret = 0;

out:
    if (week_day != NULL)
        free(week_day);
    if (cur_list != NULL)
        free(cur_list);
    return ret;
}

static int db_maintainer_cleanup_schedule()
{
    pkg_details *cur_list;
    int total_pkgs = 0, num_pkgs = 0, index;
    long cur_time = (long) now_secs();

    total_pkgs = get_total_pkgs();
    cur_list = (pkg_details *) malloc(total_pkgs * sizeof(pkg_details));

    if(cur_list == NULL)
    {
        return -1;
    }

    num_pkgs = get_pkg_list(cur_list, total_pkgs);

    if (num_pkgs == 0)
    {
        free(cur_list);
        return -1;
    }

    for (index = 0; index < num_pkgs; index++)
    {
        if(cur_time - cur_list[index].last_time_launched >= pkg_retention_threshold)
        {
            remove_pkg(cur_list[index].pkg_name);
        }
    }
    free(cur_list);
    return 0;
}

void * db_maintainer_cleanup_thread(void * arg) {
    int ret;
    arg = NULL;
    long cur_time = (long) now_secs();
    char property[PROPERTY_VALUE_MAX];

    while(1)
    {
        property_get("prefetcher.pkg_cleanup_intrvl",property, "2880");
        pkg_cleanup_interval = strtod(property, NULL) * 60;

        property_get("prefetcher.pkg_retention_mins",property, "28800");
        pkg_retention_threshold = strtod(property, NULL) * 60;

        property_get("iop.uxe.last_launch_threshold",property, "21600");
        last_launch_threshold = strtod(property, NULL) * 60;

        if ( pkg_cleanup_interval  <= 0 || pkg_retention_threshold <= 0 || last_launch_threshold <= 0)
        {
            return NULL;
        }

        ret = db_maintainer_cleanup_schedule();
        ux_engine_db_cleanup(cur_time);
        delete_mark_files();
        clean_db();
        sleep(pkg_cleanup_interval);
    }
    return NULL;
}


/*
static int db_maintainer_update_pkg(char *pkg_name)
{
    //TBD Need to fix this
    pkg_name[0] = pkg_name[0];
    return 0;
}*/

int db_maintainer_init()
{
    if(pthread_create(&db_cleanup_thread, NULL, &db_maintainer_cleanup_thread, NULL))
    {
        return -1;
    }

    return 0;
}

