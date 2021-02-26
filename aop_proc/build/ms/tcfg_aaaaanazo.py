#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2010 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
   return env.Detect('tcfg_AAAAANAZO_data')

def generate(env):
   # Save the tcfg_bid_data in the env
   env['TCFG_BID_IMAGE'] = 'AOP_PROC'

