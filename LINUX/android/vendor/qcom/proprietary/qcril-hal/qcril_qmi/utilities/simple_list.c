/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  simple_list.c

  @brief
  List implemented as a linked list

*/

#include <stdlib.h>
#include <string.h>

#include "simple_list.h"
#include "ref_counted.h"

struct simple_list;

struct simple_list_iter
{
    REF_COUNTED_DECL;
    simple_list_iter *prev;
    simple_list_iter *next;
    void *value;
    simple_list *list;
};

struct simple_list
{
    simple_list_iter *head;
    simple_list_iter *tail;
    simple_list_item_free_cb free_cb;
};

simple_list *simple_list_new()
{
    simple_list *list = calloc(1, sizeof(simple_list));
    return list;
}

simple_list *simple_list_new_with_free(simple_list_item_free_cb free_cb)
{
    simple_list *ret = simple_list_new();

    if (ret)
    {
        ret->free_cb = free_cb;
    }
    return ret;
}

static void simple_list_iter_free(void *strct, ref_counted *rfcnt)
{
    if (!strct || !rfcnt) return;

    simple_list_iter *iter = (simple_list_iter *)strct;
    if(iter->list && iter->list->free_cb)
    {
        iter->list->free_cb(iter->value);
    }
    free(iter);
}

static simple_list_iter *simple_list_iter_new()
{
    simple_list_iter *iter;

    iter = calloc(1, sizeof(simple_list_iter));
    REF_COUNTED_INIT(iter, simple_list_iter_free);
    return iter;
}

static int simple_list_valid(simple_list *list)
{
    int ret = 1;
    if ((!list) || /* Null list pointer provided */
        /* Inconsistent list. If tail is set, head must be set also */
        (!list->tail != !list->head)
       )
    {
        ret = 0;
    }
    return ret;
}

void simple_list_append(simple_list *list, void *value)
{
    simple_list_iter *new_it;

    if (!simple_list_valid(list))
    {
        return;
    }

    new_it = simple_list_iter_new();
    if(new_it == NULL)
    {
        return;
    }
    new_it->value = value;
    new_it->list = list;
    REF_COUNTED_REPL(new_it->prev, list->tail);

    if (list->tail)
    {
        list->tail->next = REF_COUNTED_INCR(new_it);
    }
    REF_COUNTED_REPL(list->tail, new_it);
    if (!list->head) list->head = REF_COUNTED_INCR(new_it);

    /* Drop the local reference */
    REF_COUNTED_DECR(new_it);
}

void simple_list_iter_delete(simple_list_iter *it)
{
    simple_list_iter *tmp = NULL;

    REF_COUNTED_REPL(tmp, it);
    if (it->prev)
    {
        REF_COUNTED_REPL(it->prev->next, it->next);
    }
    if (it->next)
    {
        REF_COUNTED_REPL(it->next->prev, it->prev);
    }
    REF_COUNTED_REPL(it->prev, (simple_list_iter *)NULL);
    REF_COUNTED_REPL(it->next, (simple_list_iter *)NULL);

    REF_COUNTED_REPL(tmp, (simple_list_iter *)NULL);
}

void simple_list_delete(simple_list *list)
{
    simple_list_iter *head = NULL, *tail = NULL, *next = NULL;

    if(!simple_list_valid(list))
    {
        return;
    }

    if (list->head)
    {
        REF_COUNTED_REPL(head, list->head);
        REF_COUNTED_REPL(list->head, (simple_list_iter *)NULL);
    }

    if (list->tail)
    {
        REF_COUNTED_REPL(tail, list->tail);
        REF_COUNTED_REPL(list->tail, (simple_list_iter *)NULL);
    }

    for (;head; REF_COUNTED_REPL(head, next))
    {
        REF_COUNTED_REPL(next, head->next);
        simple_list_iter_delete(head);
    }
    REF_COUNTED_REPL(head, (simple_list_iter *)NULL);
    REF_COUNTED_REPL(tail, (simple_list_iter *)NULL);

    free(list);
}
void simple_list_foreach(simple_list *list, simple_list_foreach_cb cb, void *userdata)
{
    simple_list_iter *it;
    if (!list || !cb)
    {
        return;
    }

    for(it = REF_COUNTED_INCR(list->head); it; REF_COUNTED_REPL(it, it->next))
    {
        if (cb(list, it, it->value, userdata))
        {
            break;
        }
    }
}

simple_list_iter *simple_list_first(simple_list *list)
{
    simple_list_iter *ret = NULL;

    if (list)
    {
        ret = REF_COUNTED_INCR(list->head);
    }
    return ret;
}

simple_list_iter *simple_list_iter_next(simple_list_iter *iter)
{
    REF_COUNTED_REPL(iter, iter->next);
    return iter;
}

void *simple_list_iter_get_value(simple_list_iter *iter)
{
    return iter->value;
}

void simple_list_iter_done(simple_list_iter *iter)
{
    REF_COUNTED_DECR(iter);
}
