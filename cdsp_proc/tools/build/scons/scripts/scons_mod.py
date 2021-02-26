'''
   scons_mod
'''
#
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/scons_mod.py#3 $
#  $DateTime: 2015/02/06 00:03:43 $
#
#===============================================================================

import os
import errno
import glob
import sys
import hashlib
import subprocess
import stat
import string
import tempfile
import SCons.Action
import SCons.Errors
import SCons.Tool
import re
import threading
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('scons_mod')

def generate(env):
    '''
       generate
    '''
    # relative paths
    env.AddMethod(rel_path, "SubstRelPath")
    env.AddMethod(rel_path, "RelPath")

    # temp file class
    init_tempfile(env)
    env.AddMethod(init_tempfile, "InitTempfile")
   
    # spawn class
    init_spawn(env)
    env.AddMethod(init_spawn, "InitSpawn")

    return None

#===============================================================================
# rel_path
# Substitue envirometn variables and returns rellative path
# substitute as is not in python 2.5.2
#===============================================================================
def rel_path(env, target, base=os.curdir, posix=False):
    '''
       rel_path
       
       Substitue envirometn variables and returns rellative path
       substitute as is not in python 2.5.2
    '''
    if env.get('PLATFORM', 'none') == 'cygwin':
        posix = True

    # substitute any env variables
    target = env.RealPath(target, posix)
    base = env.RealPath(base, posix)
    sep = env.NormPath("/", posix)

    # separate the path componets
    base_paths = base.split(sep)
    target_paths = target.split(sep)

    # check for drive only necessary on windows but it won't break in Lix*
    if (target[0].lower() != base[0].lower()):
        # raise RuntimeError, "Target path [%s] and base path[%s] are not in " \
        #                     "the same drive!" % (target,base)
        # no relative path possible return target as is..
        return target
   

    #import pdb; pdb.set_trace()
    on_windows = env.get('PLATFORM', 'none').startswith("win")
    for index in range(min(len(target_paths), len(base_paths))):
        if on_windows:
            targ_elem = target_paths[index].lower()
            base_elem = base_paths[index].lower()
        else:
            targ_elem = target_paths[index]
            base_elem = base_paths[index]
        if targ_elem != base_elem:
            break
    else:
        # If we broke out of the loop, index is pointing to the first differing
        # path elements. If we didn't break out of the loop, index is pointing 
        # to identical path elements. Increment index so that in all cases it
        # points to the first differing path elements.
        index += 1
   
    rel_paths = [os.pardir] * (len(base_paths) - index) + target_paths[index:]
    relative_path = sep.join(rel_paths)
    return relative_path

#===============================================================================
# Temp File quite
# Substitute from SCons, this version respects the verbose levels, aswell
# as not calling the system for temp files, it uses the targetname to
# get a unique filename. It also closes the file so there are no files
# handles left open.
#===============================================================================
def init_tempfile(env):
    '''
       init_tempfile
    '''
    env["TEMPFILE"] = TempFileMungeQuiet

class TempFileMungeQuiet:
    '''
       A callable class.  You can set an Environment variable to this,
       then call it with a string argument, then it will perform temporary
       file substitution on it.  This is used to circumvent the long command
       line limitation.

       Example usage:
           env["TEMPFILE"] = TempFileMungeQuiet
           env["LINKCOM"] = "${TEMPFILE('$LINK $TARGET $SOURCES')}"

       By default, the name of the temporary file used begins with a
       prefix of '@'.  This may be configred for other tool chains by
       setting '$TEMPFILEPREFIX'.

       env["TEMPFILEPREFIX"] = '-@'        # diab compiler
       env["TEMPFILEPREFIX"] = '-via'      # arm tool chain
    '''
    def __init__(self, cmd):
        '''
           __init__
        '''
        #import pdb; pdb.set_trace()   
        self.cmd = cmd
        self.verbose = int(GetOption('verbose'))
        self.frommake = int(GetOption('frommake'))
        self.use_os_temp_file = False

    def __call__(self, target, source, env, for_signature):
        '''
           __call__
        '''
        if for_signature:
            # If we're being called for signature calculation, it's
            # because we're being called by the string expansion in
            # Subst.py, which has the logic to strip any $( $) that
            # may be in the command line we squirreled away.  So we
            # just return the raw command line and let the upper
            # string substitution layers do their thing.
            return self.cmd

        # Now we're actually being called because someone is actually
        # going to try to execute the command, so we have to do our
        # own expansion.
        #import pdb; pdb.set_trace()
        cmd = env.subst_list(self.cmd, SCons.Subst.SUBST_CMD, target, source)[0]

        try:
            maxline = int(env.subst('$MAXLINELENGTH'))
        except ValueError:
            maxline = 2048

        #if (reduce(lambda x, y: x + len(y), cmd, 0) + len(cmd)) <= maxline:
        #   return self.cmd

        the_cmd = string.join(map(SCons.Subst.quote_spaces, cmd), " ")

        if len(the_cmd) <= maxline:
            return self.cmd

        if 'MAX_TOOL_LINELENGTH' in env:
            maxtoolline = int(env.subst('$MAX_TOOL_LINELENGTH'))
            cmd_length = len(the_cmd)
            if cmd_length >= maxtoolline:
                env.PrintError('** Total Command Length Exceeded(%d) Max Allowed(%d)' \
                % (cmd_length, maxtoolline))
                Exit(1)

        # We do a normpath because mktemp() has what appears to be
        # a bug in Windows that will use a forward slash as a path
        # delimiter.  Windows's link mistakes that for a command line
        # switch and barfs.
        #
        # We use the .lnk suffix for the benefit of the Phar Lap
        # linkloc linker, which likes to append an .lnk suffix if
        # none is given.

        # since we know the target no need to ask the system for a tmp file,
        # use target name as base for a tmp file.

        #import pdb; pdb.set_trace()
        #target_file = \
        #    env.subst_list("$TARGET", SCons.Subst.SUBST_CMD, target, source)[0]
        #target_path = os.path.realpath(str(target_file[0]))
        #target_tmp = target_path + ".tmp"

        #import pdb; pdb.set_trace()
        target_tmp_ext = '.scons.tmp'
        if self.use_os_temp_file:
            target_os_tmp = tempfile.mkstemp(suffix=target_tmp_ext)
            native_tmp = SCons.Util.get_native_path(target_os_tmp[1])
        else:
            target_file = os.path.realpath(str(target[0]))
      
            target_tmp = target_file + target_tmp_ext
            native_tmp = SCons.Util.get_native_path(target_tmp)

        #import pdb; pdb.set_trace()
        if env.has_key('TEMPFILEPREFIX'):
            prefix = env['TEMPFILEPREFIX']
        else:
            prefix = '@'
         
        if env.has_key('TEMPFILESUFFIX'):
            suffix = env['TEMPFILESUFFIX']
        else:
            suffix = ''
         
        # Open file and prepare commands args string
        args = map(SCons.Subst.quote_spaces, cmd[1:])
        if self.use_os_temp_file:
            file_item = os.fdopen(target_os_tmp[0], 'w')
        else:
            file_item = open(target_tmp, 'w')
      
        args_str = string.join(args, " ")
      
        # check for shell type and fix tmp file string
        #import pdb; pdb.set_trace()
        if str(env.get('SHELL', 'none')).find('sh') != -1:
            # The sh shell will try to escape the backslashes in the
            # path, so unescape them.

            if env.get('PLATFORM', 'none') == 'cygwin':
                native_tmp = string.replace(native_tmp, '\\', '/')
            
                # in cygwin fix spaces for qotes because it's going into a file
                # use case 1 comes from: CUST_H="\\"${CUST_H}\\""
                args_str = string.replace(args_str, '"\\"', '"')
                args_str = string.replace(args_str, '\\""', '"')
            
                # use case 2 comes from: CUST_H=\\"${CUST_H}\\"
                args_str = string.replace(args_str, '\\"', '"')
            else:
                native_tmp = string.replace(native_tmp, '\\', r'\\\\')
            # In Cygwin, we want to use rm to delete the temporary
            # file, because del does not exist in the sh shell.
            remove = env.Detect('rm') or 'del'
        else:
            # Don't use 'rm' if the shell is not sh, because rm won't
            # work with the Windows shells (cmd.exe or command.com) or
            # Windows path names.
            remove = 'del'
      
        # write commands args string to file_item
        file_item.write(args_str)

        # ok to close since we are not using system temp file_items
        file_item.close()

        # XXX Using the SCons.Action.print_actions value directly
        # like this is bogus, but expedient.  This class should
        # really be rewritten as an Action that defines the
        # __call__() and strfunction() methods and lets the
        # normal action-execution logic handle whether or not to
        # print/execute the action.  The problem, though, is all
        # of that is decided before we execute this method as
        # part of expanding the $TEMPFILE construction variable.
        # Consequently, refactoring this will have to wait until
        # we get more flexible with allowing Actions to exist
        # independently and get strung together arbitrarily like
        # Ant tasks.  In the meantime, it's going to be more
        # user-friendly to not let obsession with architectural
        # purity get in the way of just being helpful, so we'll
        # reach into SCons.Action directly.
      
        if self.use_os_temp_file or not self.frommake:
            rev = [ cmd[0], prefix + native_tmp + suffix + ' \n' + 
                           remove, native_tmp ]
            verbose_cmd_str = "[\'" + str(cmd[0]) + " " + prefix + \
                native_tmp + suffix + "\'],[\'" + remove + " " + \
                native_tmp + "\']"
        else:
            rev = [ cmd[0], prefix + native_tmp + suffix ]
            verbose_cmd_str = "[\'" + str(cmd[0]) + " " + prefix + \
                              native_tmp + suffix + "\']"
      
        if SCons.Action.print_actions and self.verbose > 1:
            print "** Using tempfile "+ native_tmp
            print "** CMD: " + verbose_cmd_str

        #import pdb; pdb.set_trace()
      
        return rev

#===============================================================================
# Spawn replacement
# uses Peopen isntead of launching a system shell, this has the advantage
# of having a cmd chars limit of 32k instead of 8k on ashell.
# this is important when having a lot of include paths as we do. As it ends up
# creating a lot of temp files to deal with the massive cmd line lenght.
#===============================================================================
def init_spawn(env):
    '''
       init_spawn
    '''
    if sys.platform == 'win32':
        win32_spawn = Spawn(env)
        env['SPAWN'] = win32_spawn.spawn

class Spawn:
    '''
       class Spawn
       
       Spawn replacement
       uses Peopen isntead of launching a system shell, this has the advantage
       of having a cmd chars limit of 32k instead of 8k on ashell.
       this is important when having a lot of include paths as we do. As it ends
       up creating a lot of temp files to deal with the massive cmd line length.
    '''
    def __init__(self, env):
        '''
           __init__
        '''
        self.silent = int(GetOption('silent'))
        self.verbose = int(GetOption('verbose'))
        self.serialized_stdout = int(GetOption('serial_stdout'))
        self.stdout_lock = threading.RLock()
        self.env = env

    def spawn(self, sh, escape, cmd, args, env):
        '''
           spawn
        '''
        newargs = string.join(args[1:], ' ')
        use_shell = False
      
        # clean environment,
        # it must contain only strings otherwise popen will fail
        if env:
            env_clean = {}      
            for key in env.keys():
                env_clean[key] = str(env[key])
            env = env_clean

        # fix cmd slashes for windows or get cygwin cmd
        #import pdb; pdb.set_trace()
        if self.env.get('PLATFORM', 'none') == 'cygwin':
            get_cygwin_cmd = self.env['GET_CYGWIN_CMD']
            cmdline, env = \
                get_cygwin_cmd(sh, self.env['ESCAPE'], cmd, args, env)
            #cmdline = "sh -c 'export'"
            #print cmdline
        else:
            cmd = str(cmd).replace('/', '\\')
            cmdline = cmd + " " + newargs

        if cmd == 'del':
            if os.path.exists(newargs):
                os.unlink(newargs)
            return 0

        startupinfo = subprocess.STARTUPINFO()

        # check for shell operations
        if re.search(r"(<|>|&&|\|)", cmdline):
            use_shell = True

        try:
            #import pdb; pdb.set_trace()
            proc = subprocess.Popen(
                cmdline,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                #stderr=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                startupinfo=startupinfo,
                shell = use_shell
                #env=env
            )
            #stderr = \
            #subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)

            stdout_cap = ""
            stdout_line = proc.stdout.readline()
            while(stdout_line):
                if self.serialized_stdout:
                    # Capture all stdout lines into a single string variable
                    stdout_cap += stdout_line
                else:
                    # If we're not in the special serialization mode,
                    # print to stdout now
                    print stdout_line.rstrip()
                # Read the next line
                stdout_line = proc.stdout.readline()
            # Wait for the process to exit and get exit code
            rev = proc.wait()

            # If we're serializing stdout, wrap with lock/unlock
            if self.serialized_stdout and len(stdout_cap) > 0 :
                # Lock
                self.stdout_lock.acquire()
                # If we got an error, spit out the command-line
                if rev:
                    error  = "\n\n"
                    error += "Error   -------------------\n"
                    error += "SCons Command Line:\n"
                    error += cmdline + "\n"
                    error += "\n"
                    # Print the error to STDERR
                    sys.stderr.write(error)
                # Print all stdout captured to real stdout
                print stdout_cap.rstrip()
                # Unlock
                self.stdout_lock.release()
         
        except:
            print("----------------------------------------------------------" \
                  "---------------------")
            print("*** class Spawn, Popen exception.")
            print("*** CMD:%s" % cmdline)
            print("*** OS Name=%s" % os.name)
            print("*** startupinfo = %s" % str(startupinfo))
            print("*** sys exec info:")
            print("%s" % str(sys.exc_info()))
            print("----------------------------------------------------------" \
                "---------------------")
            raise RuntimeError, 'class Spawn, Exception from subprocess.Popen'
      
        return rev
