/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  simple_list.h

  @brief
  List implemented as a linked list

*/

#ifndef SIMPLE_LIST_H_INCLUDED
#define SIMPLE_LIST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

struct simple_list;
struct simple_list_iter;

typedef struct simple_list simple_list;
typedef struct simple_list_iter simple_list_iter;

typedef int (*simple_list_foreach_cb)(simple_list *list, simple_list_iter *iter, void *value, void *user_data);
simple_list *simple_list_new();
void simple_list_delete(simple_list *list);
void simple_list_append(simple_list *list, void *value);
void simple_list_foreach(simple_list *list, simple_list_foreach_cb cb, void *userdata);
simple_list_iter *simple_list_first(simple_list *list);
simple_list_iter *simple_list_iter_next(simple_list_iter *iter);
void *simple_list_iter_get_value(simple_list_iter *iter);
void simple_list_iter_done(simple_list_iter *iter);
typedef void (*simple_list_item_free_cb)(void *item);
simple_list *simple_list_new_with_free(simple_list_item_free_cb free_cb);

#ifdef __cplusplus
}
#endif

#endif
