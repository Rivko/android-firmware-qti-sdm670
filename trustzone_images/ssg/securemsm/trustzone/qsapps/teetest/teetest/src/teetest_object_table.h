/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

#ifndef __TEETEST_OBJECT_TABLE_H__
#define __TEETEST_OBJECT_TABLE_H__

int tee_object_table_add(Object o);
Object tee_object_table_recover(int handle);

#endif // __TEETEST_OBJECT_TABLE_H__
