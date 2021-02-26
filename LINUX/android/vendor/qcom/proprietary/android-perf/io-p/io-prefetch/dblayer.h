/******************************************************************************
  @file    dblayer.h
  @brief   nheader  file to database of files

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

/******************************************************************************
  DESCRIPTION

     pkg_file_tbl               pkg_tbl
  |-----------------|      |-----------------|
  |  pkg_name       |      |  pkg_name       |
  |  file_name      |      |-----------------|
  |-----------------|      | pkg_use_time    |
  |                 |      | num_of_launches |
  | file_use_ctr    |      |-----------------|
  | file_time_stamp |
  | file_size       |
  | mark_for_delete |
  | file_modify_time|
  | file_iop_size   |
  | study_finish    |
  | mincore_array   |
  | cache_dropped   |
  | disabled        |
  |-----------------|
******************************************************************************/

/******************************************************************************
      ux_pkg_tbl                 ux_lat_tbl
  |--------------------|        |-----------------|
  |  pkg_name          |        |    pkg_name     |
  |  week_day          |        |-----------------|
  |--------------------|        |   bindApp_dur   |
  | pkg_last_use       |        |   disAct_dur    |
  | num_of_launches    |        |  wakeLock_dur   |
  | num_of_sublaunches |        |    memory_st    |
  | timeslot_1_count   |        |    bindApp_ct   |
  | timeslot_2_count   |        | predict_success |
  | timeslot_3_count   |        |   predict_fail  |
  | timeslot_4_count   |        |    not_game     |
  |--------------------|        |-----------------|
******************************************************************************/

#ifndef __PREFETCHER_DBLAYER_H__
#define __PREFETCHER_DBLAYER_H__

#include <time.h>

#define FILE_NAME_LEN 4096
#define PKG_NAME_LEN 2048
typedef enum {
    NEED_MARK_CACHE_NOT_DROPPED = -1,
    NEED_MARK_CACHE_DROPPED     = -2
} FILE_CACHE_ACTION;

typedef enum {
    NEED_MARK_FOR_DELETE_FILE   = -1,
    NEED_MARK_FOR_DELETE_PKG    = -2,
    NEED_MARK_FOR_UPDATE        = -3,
    NEED_MARK_FOR_DISABLE       = -4
} PKG_FILE_ACTION;

#define PREFERRED_APP_COUNT 8

/* data structure */
typedef struct _file_details
{
    char pkg_name[PKG_NAME_LEN];
    char file_name[FILE_NAME_LEN];
    int file_use_counter;
    int filesize; // size in KB
    int mark_for_delete;
    int file_time_stamp; // time in ms when this file open during launch
    int file_modify_time;
    int file_iop_size;
    int study_finish;
    char *mincore_array;
    int cache_dropped;
    int disabled;
} file_details;

typedef struct _pkg_details {
    char pkg_name[PKG_NAME_LEN];
    time_t last_time_launched;
    int num_launches;
} pkg_details;

typedef struct _pkg_ux_details {
    char pkg_name[PKG_NAME_LEN];
    char week_day[6];
    time_t last_time_launched;
    int num_launches;
    int num_sub_launches;
    int timeslot_count_1;
    int timeslot_count_2;
    int timeslot_count_3;
    int timeslot_count_4;
    int timeslot_count_select;
} pkg_ux_details;

typedef struct _pkg_ux_top_details {
    char pkg_name[PKG_NAME_LEN];
    char week_day[6];
    time_t last_time_launched;
    int num_launches;
    int timeslot_count_1;
    int timeslot_count_2;
    int timeslot_count_3;
    int timeslot_count_4;
    int timeslot_count_select;
    int bindApp_dur;
    int disAct_dur;
    int memory_st;
} pkg_ux_top_details;

typedef struct _pkg_ux_lat_details {
    char pkg_name[PKG_NAME_LEN];
    int bindApp_dur;
    int disAct_dur;
    int memory_st;
    int bindApp_ct;
} pkg_ux_lat_details;

typedef struct _pkg_ux_lat_pred_details {
    char pkg_name[PKG_NAME_LEN];
    float pred_success;
    float pred_fail;
} pkg_ux_lat_pred_details;

/* Public API */
int get_file_list(char * pkg_name, file_details *fl_list,int fl_list_size);
int get_pkg_list(pkg_details *pkg_list_ptr,int size);
int remove_pkg(char * pkg_name);
int remove_file(char * pkg_name,char * file_name);
int mark_for_delete(char * pkg_name,char * file_name);
int mark_for_restudy(char * pkg_name,char * file_name);
int commit_filelist_info(file_details *file_info_list, int size);
int delete_mark_files();
int update_file_details(char * pkg_name,file_details *file_info[], int size);
int update_pkg_details(pkg_details pkg_info);
int get_total_pkgs();
int get_total_file(char *pkg_name);
int get_file(char *pkg_name, char *file_name, file_details *file_info);
int get_package(char *pkg_name, pkg_details *pkg_info);
int clean_db();
int create_database();
int remove_ux_pkg(char *pkg_name, char *week_day);
int get_top_ux_pkg_list(pkg_ux_top_details *pkg_list, char *week_day, int time_slot, int size, int start);
int get_total_ux_pkgs(int uniq);
int update_ux_pkg_details(pkg_ux_details pkg_info, char *week_day, int time_slot, int sub);
int reset_ux_pkg_details(pkg_ux_details pkg_info);
int get_ux_package(char *pkg_name);
int get_ux_pkg_list(pkg_ux_details *pkg_list,int size);
double get_ux_launch_count_avg(char *week_day);
int get_total_pkg_use_count(char *pkg_name);
int get_ux_lat_pkg(char *pkg_name, pkg_ux_lat_details *pkg_info);
int update_ux_lat_details(char *pkg_name, int bindApp_dur, int disAct_dur, int memory, int non_empty);
int update_ux_lat_pred_details(char *pkg_name, int succ, int weight);
int update_ux_lat_game_details(char *pkg_name, int not_game);
int get_total_pkg_bindapp_count(char *pkg_name);
int set_ux_lat_app_upd(char *pkg_name);
int get_ux_lat_pred(pkg_ux_lat_pred_details *pkg_info);
int uninstall_pkg(char *pkg_name);
#endif //__PREFETCHER_DBLAYER_H__
