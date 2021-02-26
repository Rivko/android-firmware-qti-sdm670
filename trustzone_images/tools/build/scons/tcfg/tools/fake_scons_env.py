#===============================================================================
#
# Fake SCons Construction Environment
#
# GENERAL DESCRIPTION
# 
#    This tool defines a fake SCons environment that can be used to load
#    SCons scripts.
# 
#    Not all SCons Construction Environment functionality is implented
#    in this module.  So far it's only enough to load the Target Configurator,
#    the Target Build Config (tbc) scripts, and the incpaths<bid>.py and
#    uses<bid>.py scripts from a CoreBSP-SCons build with target-level make.
# 
# PARAMETERS:
# 
#    None.
# 
# OUTPUT:
# 
#    None.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/tcfg/tools/fake_scons_env.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/10/10   dlb'    Initial version.
#
#===============================================================================

import sys
import os
import imp

#------------------------------------------------------------------------
# Class FakeSConsEnv
# 
# This class defines a fake SCons Construction Environment that can be
# used to interface into the Target Configurator and Target Build Config
# SCons modules.
#------------------------------------------------------------------------
class FakeSConsEnv(dict):
   def __init__(self, build_root):
      self['BUILD_ROOT'] = build_root

   def subst(self, string):
      pass

   # The following is copied straight from the SCons Enfironment.py script.
   def AddMethod(self, function, name=None):
       """
       Adds the specified function as a method of this construction
       environment with the specified name.  If the name is omitted,
       the default name is the name of the function itself.
       """
       method = MethodWrapper(self, function, name)

   def PathExists (self, path):
      # Return True, assume the file exists.
      return True

   def CompareAndUpdateMD5File (self, name, md5):
      # Return False, we always want to re-gen the files.
      return False

   def PrintInfo (self, string):
      print string

   def LoadToolScript (self, toolname, toolpath):
      # Code copied piece-meal from the CoreBSP utils.py script.
      m_fp, m_path, m_desc = imp.find_module(toolname, toolpath)
      m = imp.load_module(toolname, m_fp, m_path, m_desc)
      m.generate(self)

   #----------------------------------------------------------------
   # The following functions are copied directly from target_config.py
   #----------------------------------------------------------------
   def GetTcfgImage (env):
      return env['TCFG_BID_IMAGE']

   def AddUsesFlags (env, flags, add = True):
      if type(flags) != list:
         flags = [flags]
      if add:
         for flag in flags:
            env[flag] = True
      else:
         try:
            for flag in flags:
               del env[flag]
         except:
            pass

   def GetUsesFlag (env, flag):
      return flag in env

   def AddTargetIncpaths (env, **incpaths):
      env.update(incpaths)

   def Replace (env, **incpaths):
      env.update(incpaths)

#=====================================================================
# The following is copied straight from the SCons Environment.py script.
#=====================================================================

class MethodWrapper:
    """
    A generic Wrapper class that associates a method (which can
    actually be any callable) with an object.  As part of creating this
    MethodWrapper object an attribute with the specified (by default,
    the name of the supplied method) is added to the underlying object.
    When that new "method" is called, our __call__() method adds the
    object as the first argument, simulating the Python behavior of
    supplying "self" on method calls.

    We hang on to the name by which the method was added to the underlying
    base class so that we can provide a method to "clone" ourselves onto
    a new underlying object being copied (without which we wouldn't need
    to save that info).
    """
    def __init__(self, object, method, name=None):
        if name is None:
            name = method.__name__
        self.object = object
        self.method = method
        self.name = name
        setattr(self.object, name, self)

    def __call__(self, *args, **kwargs):
        nargs = (self.object,) + args
        return apply(self.method, nargs, kwargs)

    def clone(self, new_object):
        """
        Returns an object that re-binds the underlying "method" to
        the specified new object.
        """
        return self.__class__(new_object, self.method, self.name)

#=====================================================================
# Code to test this module
#=====================================================================

#tcfg_tools_path = os.path.dirname(__file__)
#local_build_root = tcfg_tools_path.replace('tools/build/scons/tcfg/tools', '')  # in case we're on linux
#local_build_root = tcfg_tools_path.replace('tools\\build\\scons\\tcfg\\tools', '')  # in case we're on windows
#
#def test_func (env):
#   print "In test func"
#
#import pdb; pdb.set_trace()
#foo = MyFakeSConsEnv(local_build_root)
#
#foo.AddMethod(test_func, "TestFunc")
#
#foo.TestFunc()

