import sys
import os
import re
import shutil
import subprocess
from subprocess import call
from SCons.Script import *

def exists(env):
   return env.Detect('Qshrink_builder')


def generate(env):
   qshrinkutil_act = env.GetBuilderAction(Qshrink_execution)
   qshrinkutil_bld = env.Builder(action = qshrinkutil_act)
   env.Append(BUILDERS = {'QShrinkBuilder' : qshrinkutil_bld})
   
def Qshrink_execution(target, source, env):
   ELFFILEPATH=env.RealPath(str(source[0]))      
   QSHRINKPATH = '../../qdsp6/qshrink/src/Qshrink20.py'
   ELFOUTFILEPATH = env.RealPath(str(target[0]))
   HASHFILEPATH = '../../qdsp6/qshrink/src/msg_hash.txt'
   LOGFILEPATH = '../../qdsp6/qshrink/src/qsr_.txt'
   RetCode = call(['python',
                    '-O',
                    QSHRINKPATH,
                    ELFFILEPATH,
                    '--output=' + ELFOUTFILEPATH,
                    '--hashfile=' + HASHFILEPATH,
                    '--log=' + LOGFILEPATH])
   return
