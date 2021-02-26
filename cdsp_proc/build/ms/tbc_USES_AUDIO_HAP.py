
#============================================================================
#  Name:                                                                     
#    tbc_USES_AUDIO_HAP.py
#
#  Description:                                                              
#    Auto-generated file for setting pack USES flags. 
#                                                                            
# Copyright (c) 2013 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
#============================================================================
#                                                                            
# *** AUTO GENERATED FILE - DO NOT EDIT                                      
#                                                                            
#============================================================================
def exists(env):
   return env.Detect('tbc_USES_AUDIO_HAP.py')

def generate(env):
   env.AddUsesFlags('USES_AUDIO_HAP',  True)
