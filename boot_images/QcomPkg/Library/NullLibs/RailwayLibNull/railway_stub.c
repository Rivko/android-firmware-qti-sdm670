// railway_stub.c - stubbed implementation of Railway Driver
//
// Copyright 2011 - 2013,2017 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$


#include "railway.h"
#include "comdef.h"

void railway_init(void)
{
}

int rail_id(const char* rail)
{
    return 0;
}

void railway_transition_rails(void)
{}

railway_voter_t railway_create_voter(int rail, int id)
{
    return NULL;
}

void railway_corner_vote(railway_voter_t voter, railway_corner corner)
{
}

void railway_set_corner_voltage(int rail, railway_corner corner, uint32 microvolts)
{
}

uint32 railway_get_corner_voltage(int rail, railway_corner corner)
{
    return 0;
}

void railway_get_current_settings(int rail, railway_settings* settings)
{
}
