import sys
import os
import re
import shutil
import subprocess
from subprocess import call
from SCons.Script import *

def exists(env):
   return env.Detect('MemoryProc_builder')

def generate(env):
   getsecutil_act = env.GetBuilderAction(GenSectionInfo_execution)
   getsecutil_bld = env.Builder(action = getsecutil_act,emitter= GenSectionInfo_emitter)
   env.Append(BUILDERS = {'GenSectionInfo' : getsecutil_bld})

   memprocutil_act = env.GetBuilderAction(MemoryProc_execution)
   memprocutil_bld = env.Builder(action = memprocutil_act,emitter= MemoryProc_emitter)
   env.Append(BUILDERS = {'MemoryProc' : memprocutil_bld})   
   
def GenSectionInfo_execution(target, source, env):
   output=env.RealPath(str(target[0]))
   if os.path.exists(output):        
        os.remove(output) 
   for i in source:
   	ELFFILEPATH=env.RealPath(str(i))      	
   	data, err, rv = env.ExecCmds ("hexagon-llvm-objdump -h %s >> %s" % (ELFFILEPATH,output))     	
   return

def MemoryProc_execution(target, source, env): 
   MEMORYPROCPATH = env['BUILD_ROOT']+'/build/scripts/show_memory_qurt.py'
   CHIPSET = env['CHIPSET']
   IMAGENAME = env.get('IMAGE') 
   arg_len = len(source)
   OUTPUT = env.RealPath(str(target[0]))
   if os.path.exists(OUTPUT):   
       os.remove(OUTPUT)       
   INPUT = [str(i) for i in source]
   cmd = ['python', MEMORYPROCPATH, CHIPSET, IMAGENAME, OUTPUT] 
   for i in INPUT:cmd.append(i)
   RetCode = call (cmd)   
   return None

def GenSectionInfo_emitter(target, source, env):
   env.Clean(target, str(target[0]))
   return (target, source)

def MemoryProc_emitter(target, source, env):
   env.Clean(target, str(target[0]))
   return (target, source)    
