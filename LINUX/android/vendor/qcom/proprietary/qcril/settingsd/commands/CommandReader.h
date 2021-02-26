/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef COMMAND_READER_H_INCLUDED
#define COMMAND_READER_H_INCLUDED

class CommandReader
{
    public:
    Command getCommand(int fd);
};


#endif
