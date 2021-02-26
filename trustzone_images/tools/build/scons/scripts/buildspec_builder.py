#===============================================================================
#
# BUILDSPEC file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header:
#  $DateTime: 2019/02/07 03:06:10 $
#  $Author: pwbldsvc $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import os
import subprocess
import string
import re
import sys
import string
import atexit
import SCons.Action
import utils
from SCons.Script import *

sadir = ""
currentLogFile = ""
listOfTags = []
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('buildspec_builder')

def generate(env):
   buildspec_generate(env)

#===============================================================================
# PBN build rules
#===============================================================================
def buildspec_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for trace file capture
   #
   clean = env.GetOption('clean')
   if not clean:
      buildspec_act = env.GetBuilderAction(buildspec_builder)
      buildspec_bld = env.Builder(action = buildspec_act,
                                  suffix = '.bspec')
      
      env.Append(BUILDERS = {'BuildspecBuilder' : buildspec_bld})
      
      if len(SCons.Script.COMMAND_LINE_TARGETS) > 0:
      
         # builder output overrides from defaults
         progress = env.GetOption('progress')
         verbose = int(env.GetOption('verbose'))
         buildspec = str(env.GetOption('buildspec'))
         BUILDSPEC = os.environ.get('BUILDSPEC')
         target_root = str(env['TARGET_ROOT'])
         buildroot = str(env['BUILD_ROOT'])
      
         if buildspec != 'none':
            if os.path.exists(buildspec):
               topOfBuild = buildspec
            else:
               topOfBuild = target_root
         elif BUILDSPEC is not None:
            if os.path.exists(target_root):
               topOfBuild = target_root
         else:
            return
      
         sadir = os.path.normpath(topOfBuild+"/"+"static_analysis")
         if not os.path.exists(sadir):
            os.makedirs(sadir)
      
         if os.name is 'nt':
            sagenconfigTool = r'\\stone\aswcrm\smart\nt\bin\SAGenConfig.exe'
         elif os.name is 'posix':
            sagenconfigTool = r'/prj/qct/asw/qctss/linux/bin/SAGenConfig.pl'
      
         env['PRINT_CMD_LINE_FUNC'] = buildspec_print_cmd_line
         buildspec_create(sadir, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<buildspec>\n", env)
         currentLogFile = str(env['CURRENTLOGFILE'])
         if os.path.exists(sagenconfigTool):
            atexit.register(buildspec_complete, currentLogFile, env, sagenconfigTool)
         else:
             print "sagenconfig tool not found %s" % sagenconfigTool

   return None
   
#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def buildspec_emitter(target, source, env):

# here one can also add dependencies for say aexe.exe (if it needs to be build)
# env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------

def buildspec_builder(target, source, env):
# init and fix variables
   return none

#===============================================================================
# buildspec_print_cmd_line
#        - Overwrite default scons command line printer to also log all output
#          to log file in verbose mode
#        - Log commands executed to build spec for static analysis tools
#        - enabled from command line with --buildspec option
#===============================================================================
def buildspec_print_cmd_line(s, target, src, env):
   verbose = int(env.GetOption('verbose'))
   buildspec = str(env.GetOption('buildspec'))

   targetstr = string.join([str(x) for x in target]);
   srcstr = string.join([str(x) for x in src]);

   #----------------------------------------------------------------------
   # split the command str on our string identifier (&&&)
   # element one is the non-verbose output for stdout
   # element two is the entire command line with no formating
   #----------------------------------------------------------------------
   strlist = s.split("&&&");
   sys.stdout.write(" %s...\n"%strlist[0]);

   #----------------------------------------------------------------------
   # if verbose flag is on (not zero)
   # print out the entire command line to stdout 
   # currently there is no variation between verbose 1, 2, or 3
   #----------------------------------------------------------------------
   if verbose and len(strlist) > 1:
      sys.stdout.write(" %s...\n"%strlist[1]);

   #----------------------------------------------------------------------
   # not all commands have a STR list identified
   # only log those we have setup in the generate function above (i.e.: CXXCOMSTR)
   # Performance Note: We open and close this file alot it would be better
   # to save off the file handle and write to it instead of open/close
   #----------------------------------------------------------------------
   #import pdb; pdb.set_trace()
   if len(strlist) > 1:
      # Save real cmd to log file
      logstr = "<action>\n <target>" + targetstr.strip(' ') + "</target>\n <source>" + srcstr.strip(' ') + "</source>\n <command>" + strlist[1] + "</command>\n</action>\n";
      tgtStr = str(target[0])

      currentLogFile = str(env['CURRENTLOGFILE'])
      listOfTags.append(logstr)


#===============================================================================
# buildspec_complete:
#        - exit function handler to finish buildspec log, and create kw log
#===============================================================================
def buildspec_complete(currentLogFile, env, sagenconfigTool):
   buildspec = str(env.GetOption('buildspec'))
   root = os.path.normpath(str(env['TARGET_ROOT']))

#   print "USING Klocwork tool %s" % sagenconfigTool

   if os.path.exists(currentLogFile):
      if os.path.exists(currentLogFile) and os.path.getsize(currentLogFile)  > 0 and env.has_key('USES_NO_STRIP_NO_ODM'):
         for a in listOfTags:
            buildspec_writeline(currentLogFile, a,env)  
         buildspec_writeline(currentLogFile,"</buildspec>\n",env)
         buildspec_convert_to_kw(currentLogFile, env)
         file_path = currentLogFile +".trace"
         tracefile = file_path[file_path.find('static_analysis'):]
      
         if os.name is 'nt':
            sagenconfigCmd = sagenconfigTool + ' -r ' + root + " -t trace -f " + tracefile
            try:
              utils.exec_cmds(env, sagenconfigCmd)
            except NameError:
              print "call to sagenconfig failed"
         elif os.name is 'posix':
            sagenconfigCmd = "perl " + sagenconfigTool + ' -r ' + root + " -t trace -f " + tracefile
            try:
              utils.exec_cmds(env, sagenconfigCmd)
            except NameError:
              print "call to sagenconfig failed"
   else:
      print "buildspec_complete - file not found %s" % currentLogFile

#===============================================================================
# buildspec_create:
#        - helper function for creating buildspec for new run
#===============================================================================
def buildspec_create(sadir, msg, env):

   if env.has_key('IMAGE_NAME'):
      imagename = str(env['IMAGE_NAME'])
   else:
      imagename = 'none'

   buildpath=str(env['BUILDPATH'])
   if not os.path.exists(sadir+"/"+buildpath):
      os.makedirs(sadir+"/"+buildpath)
   currentLogFile = os.path.normpath(sadir+"/"+buildpath+"/"+str(SCons.Script.COMMAND_LINE_TARGETS[0])+"_"+imagename+".bspec")
   env.Replace(CURRENTLOGFILE = currentLogFile)
   filechan = open(currentLogFile, 'w')
   filechan.write(msg)
   filechan.close();

#===============================================================================
# buildspec_writeline:
#        - helper function for write log line to buildspec
#===============================================================================
def buildspec_writeline( currentLogFile, msg,env):
   if not os.path.exists(currentLogFile):
      print "buildspec_writeline - file not found %s" % currentLogFile
   else:
      filechan = open(currentLogFile, 'a')
      filechan.write(msg)
      filechan.close();


#===============================================================================
# buildspec_convert_to_kw:
#        - Parse the contents of the buildpsec XML file and convert
#          output to readable klocwork buildpsec file format
#===============================================================================
def buildspec_convert_to_kw(normalpath, env):
   import re

   buildspec = str(env.GetOption('buildspec'))

   counter = 1
   targetname = ""
   sourcename = ""
   commandname = ""
   commandfound = 0


   #----------------------------------------------------------------------
   # XML build spec format
   #  <buildpsec>
   #   <action>
   #    <target>name</target>
   #    <source>name</source>
   #    <command>command line</command>
   #   </action>
   #    ...
   #  </buildspec>
   #----------------------------------------------------------------------
   if not os.path.exists(normalpath):
      print "buildspec_writeline - file not found %s" % normalpath
   else:
      filechan = open(normalpath, 'r')
      outchan = open(normalpath+".trace", 'w')
      for line in filechan:
         line=re.sub("\s+" , " ", line)
         line=line.strip()    
         m = re.search('<target>(.*)</target>', line)
         if m is not None:
            targetname = m.group(1)
      
         m = re.search('<source>(.*)</source>', line)
         if m is not None:
            sourcename = m.group(1)
      
         m = re.search('<command>(.*)</command>', line)
         if m is not None:
            commandname = m.group(1)
            commandfound = 1
      
         if commandfound:
            commandname = commandname.strip(' ').split(' ');
            commandname[0] = os.path.normpath(commandname[0])
            commandname.insert(0, commandname[0])
            commandname = ' '.join(commandname)
            outchan.write("exec;" + str(counter) + ";" + os.getcwd() + ";" + commandname.replace(" ", ";") + "\n")
            commandfound = 0
            counter = counter + 1
      
      filechan.close()
      outchan.close()

