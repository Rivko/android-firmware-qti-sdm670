/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include "DALSys.h"
#include "inmap.h"
#include "CoreVerify.h"
#include "CoreHeap.h"

#ifdef MAX
    #undef MAX
#endif
static unsigned MAX(unsigned a, unsigned b)
{
    return (a > b) ? a : b;
}

struct inmap_s
{
    unsigned link_offset;
    unsigned key_offset;

    unsigned size;
    void    *root;
};

inmap_t *inmap_create(unsigned link_offset, unsigned key_offset)
{
    inmap_t *self = Core_Malloc(sizeof(inmap_t));
    CORE_VERIFY_PTR(self);
    memset(self, 0, sizeof(inmap_t));

    self->link_offset = link_offset;
    self->key_offset  = key_offset;
    return self;
}

void inmap_destroy(inmap_t *self)
{
    Core_Free(self);
}

unsigned inmap_size(inmap_t *self)
{
    return self->size;
}

static inmap_link_t *get_link(inmap_t *self, void *object)
{
    char *link_ptr = ((char *)object) + self->link_offset;
    return (inmap_link_t *)link_ptr;
}

static unsigned get_key(inmap_t *self, void *object)
{
    char *key_ptr = ((char *)object) + self->key_offset;
    return *((unsigned *)key_ptr);
}

static void initialize_link(inmap_t *self, void *object)
{
    get_link(self, object)->left   = NULL;
    get_link(self, object)->right  = NULL;
    get_link(self, object)->height = 1;
}

unsigned inmap_height(inmap_t *self)
{
    if(!self->root)
        return 0;

    return get_link(self, self->root)->height;
}

static void inmap_rotate_left(inmap_t *self, void *parent, void *node)
{
    void *save, **root, *left, *right;

    // figure out which child pointer of the parent is changing
    if(!parent)
        root = &self->root;
    else
        root = get_link(self, parent)->left == node ? &get_link(self, parent)->left
                                                    : &get_link(self, parent)->right;

    // new root is the right child
    *root = get_link(self, node)->right;

    // save the rotating subtree
    save = get_link(self, *root)->left;

    // new left is the node
    get_link(self, *root)->left = node;

    // move the rotating subtree
    get_link(self, node)->right = save;

    // update the rotated entity's height
    left  = get_link(self, node)->left;
    right = get_link(self, node)->right;
    get_link(self, node)->height = MAX(left  ? get_link(self, left)->height  + 1 : 1,
                                       right ? get_link(self, right)->height + 1 : 1);

    // update the new root's height
    left  = get_link(self, *root)->left;
    right = get_link(self, *root)->right;
    get_link(self, *root)->height = MAX(left  ? get_link(self, left)->height  + 1 : 1,
                                        right ? get_link(self, right)->height + 1 : 1);
}

static void inmap_rotate_right(inmap_t *self, void *parent, void *node)
{
    void *save, **root, *left, *right;

    // figure out which child pointer of the parent is changing
    if(!parent)
        root = &self->root;
    else
        root = get_link(self, parent)->left == node ? &get_link(self, parent)->left
                                                    : &get_link(self, parent)->right;

    // new root is the left child
    *root = get_link(self, node)->left;

    // save the rotating subtree
    save = get_link(self, *root)->right;

    // new root's right is the original root
    get_link(self, *root)->right = node;

    // move the rotating subtree
    get_link(self, node)->left = save;

    // update the rotated entity's height
    left  = get_link(self, node)->left;
    right = get_link(self, node)->right;
    get_link(self, node)->height = MAX(left  ? get_link(self, left)->height  + 1 : 1,
                                       right ? get_link(self, right)->height + 1 : 1);

    // update the new root's height
    left  = get_link(self, *root)->left;
    right = get_link(self, *root)->right;
    get_link(self, *root)->height = MAX(left  ? get_link(self, left)->height  + 1 : 1,
                                        right ? get_link(self, right)->height + 1 : 1);
}

static void inmap_balance_tree(inmap_t *self, void *parent, void *node)
{
    unsigned left_height, right_height;
    void *left, *right;
    int balance;

    // Balance tree and adjust height.
    left         = get_link(self, node)->left;
    left_height  = left ? get_link(self, left)->height : 0;

    right        = get_link(self, node)->right;
    right_height = right ? get_link(self, right)->height : 0;

    balance = left_height - right_height;
    switch(balance)
    {
        case -2:
            // Node-heavy on the right side
            {
                void *sub_left = get_link(self, right)->left;
                unsigned sub_left_height = sub_left ? get_link(self, sub_left)->height : 0;

                void *sub_right = get_link(self, right)->right;
                unsigned sub_right_height = sub_right ? get_link(self, sub_right)->height : 0;

                int sub_balance = sub_left_height - sub_right_height;
                switch(sub_balance)
                {
                    // The "right-left case."
                    case  1:
                        inmap_rotate_right(self, node, right);
                        // continue to the right-right case

                    // The "right-right case."
                    case -1:
                        inmap_rotate_left(self, parent, node);
                        break;
                }
            }
            break;

        case  2:
            // Node-heavy on the left side
            {
                void *sub_left = get_link(self, left)->left;
                unsigned sub_left_height = sub_left ? get_link(self, sub_left)->height : 0;

                void *sub_right = get_link(self, left)->right;
                unsigned sub_right_height = sub_right ? get_link(self, sub_right)->height : 0;

                int sub_balance = sub_left_height - sub_right_height;
                switch(sub_balance)
                {
                    // The "left-right case."
                    case -1:
                        inmap_rotate_left(self, node, left);
                        // continue to the left-left case

                    // The "left-left case."
                    case  1:
                        inmap_rotate_right(self, parent, node);
                        break;
                }
            }
            break;

        default:
            // for -1/0/1 we don't need to balance
            break;
    }
}

static inmap_insert_t _inmap_insert(inmap_t *self, void *parent, void *node, void *object)
{
    inmap_insert_t result;

    // Check for duplicate key.
    if(get_key(self, node) == get_key(self, object))
    {
        result.inserted = 0;
        result.object   = node;
        return result;
    }

    // Figure out which side of this node the new object is destined for.
    if(get_key(self, object) < get_key(self, node))
    {
        if(!get_link(self, node)->left)
        {
            initialize_link(self, object);
            get_link(self, node)->left = object;
            result.inserted = 1; result.object = object;

            // the height may have increased
            get_link(self, node)->height = MAX(get_link(self, node)->height, 2);
        }
        else
        {
            void *left, *right;
            result = _inmap_insert(self, node, get_link(self, node)->left, object);
            left  = get_link(self, node)->left;
            right = get_link(self, node)->right;
            get_link(self, node)->height = MAX(left  ? get_link(self, left)->height  + 1 : 1,
                                               right ? get_link(self, right)->height + 1 : 1);
        }
    }
    else
    {
        if(!get_link(self, node)->right)
        {
            initialize_link(self, object);
            get_link(self, node)->right = object;
            result.inserted = 1; result.object = object;

            // the height may have increased
            get_link(self, node)->height = MAX(get_link(self, node)->height, 2);
        }
        else
        {
            void *left, *right;
            result = _inmap_insert(self, node, get_link(self, node)->right, object);
            left  = get_link(self, node)->left;
            right = get_link(self, node)->right;
            get_link(self, node)->height = MAX(left  ? get_link(self, left)->height  + 1 : 1,
                                               right ? get_link(self, right)->height + 1 : 1);
        }
    }

    inmap_balance_tree(self, parent, node);
    return result;
}

inmap_insert_t inmap_insert(inmap_t *self, void *object)
{
    inmap_insert_t result;

    if(!self->root)
    {
        inmap_insert_t result;

        result.inserted = 1;
        result.object = object;

        initialize_link(self, object);
        self->root = object;
        self->size = 1;

        return result;
    }

    result = _inmap_insert(self, NULL, self->root, object);
    if(result.inserted)
        self->size += 1;

    return result;
}

void *inmap_search(inmap_t *self, unsigned key)
{
    void *node = self->root;

    while(node)
    {
        unsigned node_key = get_key(self, node);

        if(key == node_key)
            return node;
        else if(key < node_key)
            node = get_link(self, node)->left;
        else
            node = get_link(self, node)->right;
    }

    return 0;
}

static void _inmap_visit(inmap_t *self, inmap_visit_func visit_func, void *context, void *node)
{
    void *left  = get_link(self, node)->left;
    void *right = get_link(self, node)->right;

    if(left)
        _inmap_visit(self, visit_func, context, left);

    visit_func(node, context);

    if(right)
        _inmap_visit(self, visit_func, context, right);
}

void inmap_visit(inmap_t *self, inmap_visit_func visit_func, void *context)
{
    if(self->root)
        _inmap_visit(self, visit_func, context, self->root);
}

static void _inmap_visit_post(inmap_t *self, inmap_visit_func visit_func, void *context, void *node)
{
    void *left  = get_link(self, node)->left;
    void *right = get_link(self, node)->right;

    if(left)
        _inmap_visit_post(self, visit_func, context, left);

    if(right)
        _inmap_visit_post(self, visit_func, context, right);

    visit_func(node, context);
}

void inmap_visit_postorder(inmap_t *self, inmap_visit_func visit_func, void *context)
{
    if(self->root)
        _inmap_visit_post(self, visit_func, context, self->root);
}

