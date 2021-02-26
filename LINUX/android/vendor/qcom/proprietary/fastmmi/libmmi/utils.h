/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __UTILS_H
#define __UTILS_H

#include "common.h"

void create_func_map();
cb_t get_cb(string func_name);
cb_t get_cb(char *func_name);

int create_socket(const char *name);
int connect_server(const char *path);
int say_hello(int sock, const char *module_name);
int send_cmd(int sock, const char *module_name, int cmd, const char *subcmd, const char *params, int params_size);

void enqueue_msg(msg_queue_t * queue, msg_t * msg);
void dequeue_msg(msg_queue_t * queue, msg_t ** msg);

int parse_module(const char *line, char *module, int module_len);

void parse_parameter(const char *src, unordered_map < string, string > &paras);
void parse_parameter(const string src, unordered_map < string, string > &paras);
int parse_nv_by_indicator(const char *line, char indicator, char *name, int nameLen, char *value, int valueLen);
long string_to_long(const char *src);
long string_to_long(const string src);
unsigned long string_to_ulong(const string src);
unsigned long string_to_ulong(const char *src);
void string_to_lower(char *src, int size);
void string_to_upper(char *src, int size);
void string_split(const string& src, const string& separator, list<string>& dest);


int charArray_to_hexArray(char *input, int inputLen, int start, int len, char *output, int outputLen, bool revert);
int count_char(char *src, char achar);
int get_pos(char *src, int line);

int parse_value(const char *line, char indicator, char *name, int name_len, char *value, int value_len);

int send_msg(int fd, msg_t * msg);

char *trim(char *str);
void trim(string & src);

int get_device_index(char *device, const char *path, int *deviceIndex);
void signal_handler(int signal);

int clear_file(const char *filepath);
int write_file(const char *path, const char *content);
int read_file(const char *filepath, char *buf, int size);
int delete_file(const char *file,int limit);

int copy_file(char *src, char *dest);
bool check_file_exist(const char *path);
int exe_cmd(callback cb, exec_cmd_t * exec_cmd);
int fork_daemon(const char *filepath,char **args,int* cid);

bool is_proc_exist(int pid);
void kill_proc(int pid);
void kill_thread(pthread_t tid);
void *zmalloc(size_t n);
bool inside_int(int target, int min, int max);
bool inside_float(float target, float min, float max);
bool inside_long(long target, long min, long max);
int find_pid_by_name(char *proc_name, int *found_pid);
int safe_overflow_add(int x, int y, int *result);
#endif
