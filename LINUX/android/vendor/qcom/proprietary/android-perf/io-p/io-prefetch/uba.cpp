/******************************************************************************
  @file    uba.c
  @brief   Android performance User behavior analyzer thats predicts user
           preferred apps.

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/


#include "dblayer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <cutils/trace.h>
#include <time.h>

#include "uba.h"

#undef ATRACE_TAG
#define ATRACE_TAG ATRACE_TAG_ALWAYS

#undef LOG_TAG
#define LOG_TAG           "UXEngine"

#if QC_DEBUG
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#define QLOGE(...)
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#endif

palm_table *tracker;
static int old_tbl_size = 0;
static int empty_restart_threshold = 2;
int time_slot;
int pr_launch = 0;

static double now_secs(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (double)1.0 * res.tv_sec + (double) res.tv_nsec / 1e9;
}

int init_uba()
{
    tracker = (palm_table *) malloc (PREFERRED_APP_COUNT * sizeof(palm_table));
    if (tracker == NULL) {
        QLOGE("UBA Init Allocation failed.");
        return 0;
    }
    QLOGI("UBA Init Tracker allocated");
    return 1;
}

void shutdown_uba()
{
    if (tracker != NULL)
        free(tracker);
}

//int set_palm_table(char list[][PKG_NAME_LEN], int tb_size, long cur_time) {
int set_palm_table(char **list, int tb_size, long cur_time) {
    //Compare old and new list, and send update to PALM
    //Check for empty conditions.
    int i = 0, j = 0, match = 0, match_cached = 0;

    palm_table *cur_tracker;
    palm_table *tmp_tracker;
    cur_tracker = (palm_table *) malloc (tb_size * sizeof(palm_table));

    if (cur_tracker == NULL) {
        QLOGE("Set_Palm_Table: No memory to set temp table. Return");
        return 0;
    }

    QLOGI("Set_Palm_Table: Entry, Table size: %d, old table size: %d, Current time: %ld \n", tb_size, old_tbl_size, cur_time);
    for(i = 0; i < tb_size; i++)
    {
         strlcpy(cur_tracker[i].pkg_name, list[i], PKG_NAME_LEN);
         cur_tracker[i].cached = false;
         cur_tracker[i].empty = true;
         cur_tracker[i].memory_rss = 0; //Use getMemory or db call later on.
         cur_tracker[i].last_empty_time = cur_time;
         cur_tracker[i].do_not_launch = false;
         cur_tracker[i].hit = false;
         cur_tracker[i].comp = false;

         for(j = 0; j < old_tbl_size; j++)
         {
             if(!strncmp(tracker[j].pkg_name, list[i], PKG_NAME_LEN))
             {
                 cur_tracker[i].memory_rss = tracker[j].memory_rss;
                 cur_tracker[i].last_empty_time = tracker[j].last_empty_time;
                 cur_tracker[i].cached = tracker[j].cached;
                 cur_tracker[i].empty = tracker[j].empty;
                 cur_tracker[i].do_not_launch = tracker[j].do_not_launch;
                 cur_tracker[i].hit = tracker[j].hit;
                 tracker[j].comp = true;
                 strlcpy(cur_tracker[i].pkg_name, tracker[j].pkg_name, PKG_NAME_LEN);
                 if(tracker[j].do_not_launch) {
                     QLOGI("Set_Palm_Table: Not allowed for launch due to looping: %s\n", tracker[j].pkg_name);
                     list[i][0] = 0;
                     list[i][1] = '\0';
                     cur_tracker[i].cached = false;
                     cur_tracker[i].empty = false;
                     cur_tracker[i].do_not_launch = true;
                     if ((cur_time - tracker[j].last_empty_time) > empty_restart_threshold) {
                         cur_tracker[i].do_not_launch = false;
                         QLOGE("Set_Palm_Table: Timeout expired. Allowed to start as empty: %s\n", tracker[j].pkg_name);
                     }
                 }
                 if(tracker[j].empty || tracker[j].cached) {
                     list[i][0] = 0;
                     list[i][1] = '\0';
                     QLOGI("Set_Palm_Table: Matched empty from previous table. Pkg_name: %s Pkg State--Cached: %d, Empty: %d\n", tracker[j].pkg_name, tracker[j].cached, tracker[j].empty);
                     strlcpy(cur_tracker[i].pkg_name, tracker[j].pkg_name, PKG_NAME_LEN);
                     match++;
                 }
                 if(!tracker[j].empty && !tracker[j].cached) {
                     cur_tracker[i].empty = true;
                     cur_tracker[i].cached = false;
                     QLOGI("Set_Palm_Table: Matched, but app was killed. Resetting. Pkg_name: %s Pkg State--Cached: %d, Empty: %d\n", cur_tracker[i].pkg_name, cur_tracker[i].cached, cur_tracker[i].empty);
                 }
             }
         }
    }
    for(j = 0; j < old_tbl_size; j++)
    {
        if(!tracker[j].comp) {
             QLOGI("Set_Palm_Table: App being removed from tracker. App- %s\n", tracker[j].pkg_name);
             if (tracker[j].hit) {
                 update_ux_lat_pred_details(tracker[j].pkg_name, 1, 10);
             } else {
                 int f_contrib = pr_launch/PREFERRED_APP_COUNT*10;
                 update_ux_lat_pred_details(tracker[j].pkg_name, 0, f_contrib);
             }
        }
    }
    old_tbl_size = tb_size;
    //memcpy(tracker, cur_tracker, (tb_size*sizeof(palm_table)));
    tmp_tracker = tracker;
    tracker = cur_tracker;
    free(tmp_tracker);
    if (match == tb_size)
        return 0;
    else
        return (tb_size - match);

}

void update_palm_table(char *pkg_name, int kill, int launch)
{
    int i = 0;
    long cur_time = (long) now_secs();
    QLOGI("Enter Update_palm_table. Pkg_name : %s old_tbl_size %d kill: %d launch: %d\n", pkg_name, old_tbl_size, kill, launch);
    for(i = 0 ; i < old_tbl_size ; i++)
    {
        if(!strncmp(tracker[i].pkg_name, pkg_name, PKG_NAME_LEN))
        {
            QLOGI("Update_palm_table. Tracker Pkg_name : %s\n", tracker[i].pkg_name);
            if (kill) {
                if ((cur_time - tracker[i].last_empty_time) < empty_restart_threshold && !tracker[i].cached)
                {
                    //app is launch thrashing
                    tracker[i].do_not_launch = true;
                }
                tracker[i].cached = false;
                tracker[i].empty = false;
                QLOGI("Update_palm_table kill. Pkg_name : %s state-- cached: %d empty: %d\n", tracker[i].pkg_name, tracker[i].cached, tracker[i].empty);
            }
            if (launch) {
                tracker[i].cached = true;
                tracker[i].empty = false;
                tracker[i].hit = true;
                QLOGI("Update_palm_table launched. Pkg_name : %s state-- cached: %d empty: %d\n", tracker[i].pkg_name, tracker[i].cached, tracker[i].empty);
            }
        }
    }
    if (launch) {
        pr_launch++;
    }
    if (pr_launch == PREFERRED_APP_COUNT) {
        for (i = 0; i < old_tbl_size; i++)
        {
            if (tracker[i].hit) {
                update_ux_lat_pred_details(pkg_name, 1, 10);
                tracker[i].hit = false;
            } else {
                update_ux_lat_pred_details(pkg_name, 0, 10);
            }
        }
        pr_launch = 0;
    }
}

int get_preferred_apps(char **out_list)
{
    long cur_time = (long) now_secs();
    char *week_day = NULL;
    pkg_ux_top_details *cur_list = NULL;
    int total_pkgs = 0, index = 0, table_size = 0, num_pkgs = 0;
    int tmp_size = PREFERRED_APP_COUNT + PREFERRED_APP_COUNT/2;

    week_day = (char *) malloc(6*sizeof(char));
    if (week_day == NULL) {
        //Malloc failed. Most-probably low on memory.
        goto out;
    }
    compute_time_day_slot(week_day, &time_slot);
    total_pkgs = get_total_ux_pkgs(0);

    if (total_pkgs == 0) {
        goto out;
    }
    cur_list = (pkg_ux_top_details *) malloc ((tmp_size) * sizeof(pkg_ux_top_details));

    if (cur_list == NULL) {
        goto out;
    }

    QLOGI("Get_Preferred_Apps: Time stats: Week_day: %s, Time_slot: %d, Total_pkgs: %d\n", week_day, time_slot, total_pkgs);
    num_pkgs = get_top_ux_pkg_list(cur_list, week_day, time_slot, tmp_size, 1);
    QLOGI("Get_Preferred_Apps: Get Top pkg list Returned pkgs: %d\n", num_pkgs);

    if (num_pkgs == 0) {
        goto out;
    }

    for (index = 0 ; (index < PREFERRED_APP_COUNT && index < num_pkgs) ; index++) {
         int total_count = cur_list[index].num_launches;
         int tslot_count = cur_list[index].timeslot_count_select;
         QLOGI("Get_Preferred_Apps: Accessing cur list: %s pkg_launch_count: %d bindApp_dur: %d disAct_dur%d\n", cur_list[index].pkg_name, cur_list[index].num_launches,
                                                                                                                cur_list[index].bindApp_dur, cur_list[index].disAct_dur);
         strlcpy(out_list[index], cur_list[index].pkg_name, PKG_NAME_LEN);
         QLOGI("Get_Preferred_Apps: Accessing copied list: %s\n", out_list[index]);
         table_size++;
    }

    QLOGI("Get_Preferred_Apps: Accessing table_size %d\n", table_size);
    set_palm_table(out_list, table_size, cur_time);

out:
    if (cur_list != NULL) {
        free(cur_list);
    }
    if (week_day != NULL) {
        free(week_day);
    }
    return table_size;
}

void compute_time_day_slot(char *week_day, int *time_slot)
{
    time_t cur_time = time(NULL);
    struct tm *tinfo = localtime(&cur_time);
    int t_hour = tinfo->tm_hour;
    QLOGI("Compute_time_day_slot .. t_hour: %d time: %s\n", t_hour, asctime(tinfo));
    tinfo->tm_wday > 0 && tinfo->tm_wday < 6 ? strlcpy(week_day,"true",6) : strlcpy(week_day,"false",6);
    // Package day and time into a int.
    if (3 < t_hour && t_hour <= 11) {
        *time_slot = 1;
    }
    else if (11 < t_hour && t_hour <= 16)
        *time_slot = 2;
    else if (16 < t_hour && t_hour <= 20)
        *time_slot = 3;
    else
        *time_slot = 4;
    QLOGI("Compute_time_day: week_day: %s time_slot: %d\n", week_day, *time_slot);
}
