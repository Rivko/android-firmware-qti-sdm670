/*=============================================================================

                                    calc.c

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
int calculator(int x, int y, char op)
{
    int ret_val;
    switch(op)
    {
        case '+':
            ret_val = x + y;
            break;
        case '-':
            ret_val = x - y;
            break;
        case '*':
            ret_val = x * y;
            break;
        default:
            ret_val = 0;
    };
    return ret_val;
}

int calc_add(int x, int y)
{
    return (x + y);
}

int calc_sub(int x, int y)
{
    return (x - y);
}

int calc_mult(int x, int y)
{
    return (x * y);
}