#==============================================================================
# SCONS Tool for Target-Specific Build Parameters.
#
# Copyright (c) 2012-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#==============================================================================
#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //components/rel/core.qdsp6/2.1.c4/bsp/build/tbc_core_devcfg.py#1 $
#
# when       who    what, where, why
# -------    ---    -------------------------------------------
# 08/14/12   aa   Initial Revision
#==============================================================================
import xml.dom.minidom
import unicodedata



def generate(env):
   

   
   env.Append(CPPDEFINES = [])

def exists(env):
    return env.Detect('tbc_core_devcfg')
