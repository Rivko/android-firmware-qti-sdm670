'''
   genpaths
'''
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
import re
from array import array
from SCons.Script import *

def init(env):
    '''
       init
    '''
    ScanIncPaths(env)
    return None

def GenPathsDebug(env, msg):
    '''
       GenPathsDebug
    '''
    debugincpaths = env.GetOption('debugincpaths')
    if debugincpaths:
        print msg


def ScanPaths(env, filename, includes, paths):
    '''
       ScanPaths
    '''
    GenPathsDebug(env, "scanning " + filename)

    # get all vars for the file
    GenPaths(env, filename, paths)
    file_chan = open(filename, 'r')

    while (1):
        line = file_chan.readline()
        if line == "": 
            break

        pat = re.compile("^(include|-include).*")
        mat = pat.match(line)
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

    file_chan.close()


def GenPaths(env, filename, paths):
    '''
       GenPaths
    '''
    GenPathsDebug(env, "reading " + filename)
    file_chan = open(filename, 'r')

    append_flag = 0
    append_val = []
   
    while (1):
        line = file_chan.readline()
        if line == "": 
            break
     
        if append_flag and len(line) and line[len(line)-2] == "\\":
            val = line
            val = val.replace("\\", "")
            val = val.strip()
            for val_item in val.split():
                val_item = val_item.strip()
                val_item = val_item.replace("$(", "${")
                val_item = val_item.replace(")", "}")
                pat = re.compile("^\\$\\{([0-9].*)$")
                mat = pat.match(val_item)
                if mat:
                    val_item = "${V_" + mat.group(1)
                    GenPathsDebug(env, "pat matched \"" + val_item + "\"")
                val2 = env.subst(val_item)
                append_val.append(val2)
                GenPathsDebug(env, "continuation: " + str(name) + " val " + \
                	      str(val_item) + " as " + str(val2))
            continue
        else:      
            GenPathsDebug(env, "appendFlag: " + str(append_flag) + " len: " + \
            	          str(len(line)) + " last char: " + str(line[len(line)-1]))
            if append_flag:
                pat = re.compile("^[0-9]")
                mat = pat.match(name)
                if mat:
                    name = "V_" + name

            paths[name] = append_val
            try:
                env[name] = append_val
            except: # catch *all* exceptions
                exec_info = sys.exc_info()[1]
                GenPathsDebug(env, "warning: failed to set " + name  )
                print exec_info

            GenPathsDebug(env, "Name: " +name)
            GenPathsDebug(env, append_val)
         
        append_flag = 0
        pat = re.compile(".*=.*")
        mat = pat.match(line)
        if mat:
            nameval = mat.group()
            name = nameval.split("=")[0].strip()
            if name.find("#", 0, 1) != -1:
                GenPathsDebug(env, "comment: " + name + " L: " + line)
                continue

            if len(name.split()) > 1:
                name = name.split()[1]

            if name.find("SRCROOT", 0, 7) != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue
            if name.find("ROOT", 0, 4) != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue
            if name.find("+") != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue
            if name.find(":") != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue

            val = nameval.split("=")[1].strip()
            if val.find("addprefix") != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue
            if val.find("subst") != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue
            if val.find("addsuffix") != -1:
                GenPathsDebug(env, "ignored: " + name + " L: " + line)
                continue

            val = val.replace("$(", "${")
            val = val.replace(")", "}")
            if len(val) and val[len(val)-1] == "\\":
                append_flag = 1
                val = val.replace("\\", "")
                val = val.strip("/")
                append_val.append(val)
                GenPathsDebug(env, "continuation: " + name + " L: " + line)
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
                exec_info = sys.exc_info()[1]
                GenPathsDebug(env, \
                    "warning: failed to set " + name + " to " + val2 )
                print exec_info
           
            GenPathsDebug(env, "Name: " +name + " as " + val + " val2 " + val2)
        else:
            GenPathsDebug(env, "no match: " + line)
    file_chan.close()


def ScanIncPaths(env):
    '''
       ScanIncPaths
    '''
    env.Replace(SRCROOT = "../../..")
    env.Replace(ROOT = "../../..")
    includes = []
    paths = dict()

    print "Generating AMSS paths..."
    ScanPaths(env, env['BUILD_ROOT'] +'build/ms/incpaths.min', includes, paths)

    file_chan = open("paths.cache", 'w')
    for k in paths.keys():
        GenPathsDebug(env, "key: " + k)
        GenPathsDebug(env, paths[k])
        path = env.subst(paths[k])
        env.Replace(k = path)

        file_chan.write(k)
        file_chan.write(" = ")
        for path_item in path:
            file_chan.write(path_item)
        file_chan.write("\n")
      
    file_chan.close()
