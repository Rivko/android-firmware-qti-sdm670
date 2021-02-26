#===============================================================================
#                    Copyright 2009 Qualcomm Technologies Incorporated.
#                           All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#===============================================================================
# Common utils
#===============================================================================
import os
import subprocess 
import string
import sys
import re, string, os
from array import array
from SCons.Script import *

def init(env):
   ScanIncPaths(env)
   return None

def GenPathsDebug(env, msg):
   debugincpaths = env.GetOption('debugincpaths')
   if debugincpaths:
     print msg


def ScanPaths(env, filename, includes, paths):
   GenPathsDebug(env, "scanning " + filename)

   # get all vars for the file
   GenPaths(env, filename, paths)
   fileChan = open(filename, 'r')

   while (1):
      L = fileChan.readline()
      if L == "": 
         break

      pat = re.compile("^(include|-include).*")
      mat = pat.match(L)
      if mat:
         name = mat.group().split()[1]
         name = name.replace("$(", "${")
         name = name.replace(")/", "}/")
         name2 = env.subst(name.strip())
         if os.path.isfile(name2):
            name = name2
            
         if name not in includes:
            GenPathsDebug(env, "adding " + name + " to include list")
            includes.append(name)
            if os.path.isfile(name):
               ScanPaths(env, name, includes, paths)

   fileChan.close()


def GenPaths(env, filename, paths):
   GenPathsDebug(env, "reading " + filename)
   fileChan = open(filename, 'r')

   appendFlag = 0
   appendVal = []
   
   while (1):
     L = fileChan.readline()
     if L == "": 
       break
     
     if appendFlag and len(L) and L[len(L)-2] == "\\":
       val = L
       val = val.replace("\\", "")
       val = val.strip()
       for v in val.split():
         v = v.strip()
         v = v.replace("$(", "${")
         v = v.replace(")", "}")
         pat = re.compile("^\\$\\{([0-9].*)$")
         mat = pat.match(v)
         if mat:
            v = "${V_" + mat.group(1)
            GenPathsDebug(env, "pat matched \"" + v + "\"")
         val2 = env.subst(v)
         appendVal.append(val2)
         GenPathsDebug(env, "continuation: " + str(name) + " val " + str(v) + " as " + str(val2))
       continue
     else:      
       GenPathsDebug(env, "appendFlag: " + str(appendFlag) + " len: " + str(len(L)) + " last char: " + str(L[len(L)-1]))
       if appendFlag:
         pat = re.compile("^[0-9]")
         mat = pat.match(name)
         if mat:
            name = "V_" + name

         paths[name] = appendVal
         try:
            env[name] = appendVal
         except: # catch *all* exceptions
            e = sys.exc_info()[1]
            GenPathsDebug(env, "warning: failed to set " + name  )
            print e

         GenPathsDebug(env, "Name: " +name)
         GenPathsDebug(env, appendVal)
         
       appendFlag = 0
       pat = re.compile(".*=.*")
       mat = pat.match(L)
       if mat:
         nameval = mat.group()
         name = nameval.split("=")[0].strip()
         if name.find("#",0,1) != -1:
            GenPathsDebug(env, "comment: " + name + " L: " + L)
            continue

         if len(name.split()) > 1:
            name = name.split()[1]

         if name.find("SRCROOT", 0, 7) != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue
         if name.find("ROOT", 0, 4) != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue
         if name.find("+") != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue
         if name.find(":") != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue

         val = nameval.split("=")[1].strip()
         if val.find("addprefix") != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue
         if val.find("subst") != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue
         if val.find("addsuffix") != -1:
            GenPathsDebug(env, "ignored: " + name + " L: " + L)
            continue

         val = val.replace("$(", "${")
         val = val.replace(")", "}")
         if len(val) and val[len(val)-1] == "\\":
            appendFlag = 1
            val = val.replace("\\", "")
            val = val.strip("/")
            appendVal.append(val)
            GenPathsDebug(env, "continuation: " + name + " L: " + L)
            continue
         val = val.replace("\\", "")
         val2 = env.subst(val)
         pat = re.compile("^[0-9]")
         mat = pat.match(name)
         if mat:
            name = "V_" + name
            
         paths[name] = val2
         
         try:
            env[name] = Dir(val2).path.replace('\\', '/')
         except: # catch *all* exceptions
            e = sys.exc_info()[1]
            GenPathsDebug(env, "warning: failed to set " + name + " to " + val2 )
            print e
           
         GenPathsDebug(env, "Name: " +name + " as " + val + " val2 " + val2)
       else:
         GenPathsDebug(env, "no match: " + L)
   fileChan.close()


def ScanIncPaths(env):
   env.Replace(SRCROOT = "../../..")
   env.Replace(ROOT = "../../..")
   includes = []
   paths = dict()

   print "Generating AMSS paths..."
   ScanPaths(env, env['BUILD_ROOT'] +'build/ms/incpaths.min', includes, paths)

   fileChan = open("paths.cache", 'w')
   for k in paths.keys():
      GenPathsDebug(env, "key: " + k)
      GenPathsDebug(env, paths[k])
      path = env.subst(paths[k])
      env.Replace(k = path)

      fileChan.write(k)
      fileChan.write(" = ")
      for p in path:
         fileChan.write(p)
      fileChan.write("\n")
      
   fileChan.close()
