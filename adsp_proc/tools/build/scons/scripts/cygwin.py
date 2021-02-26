'''
   Cygwin helpers
   
   GENERAL DESCRIPTION
      Common utilities used across the build system   
'''
#===============================================================================
#
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 The SCons
# Foundation
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
# Cygwin helpers
#
# GENERAL DESCRIPTION
#    Common utilities used across the build system
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/cygwin.py#2 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Change: 7115037 $
#
#===============================================================================
import os
import string

#------------------------------------------------------------------------------
# Posix RDirs
# Makes sure the inc paths are posix compliant.
#------------------------------------------------------------------------------
def posix_RDirs (path_list):
    '''
       posix_RDirs

       Makes sure the inc paths are posix compliant.
    '''
    path_list = [str(path).replace('\\', '/') for path in path_list]
    return path_list

#------------------------------------------------------------------------------
# Escape
# Unescapes escapes chars for passing to sh -c "cmd args"
#------------------------------------------------------------------------------
def escape(arg):
    '''
       escape

       escape shell special characters
    '''
    slash = '\\'
    special = '"$()'

    arg = string.replace(arg, slash, slash + slash)
    for char in special:
        arg = string.replace(arg, char, slash + char)

    #return '"' + arg + '"'
    return arg

#------------------------------------------------------------------------------
# get_cygwin_command
# Get the cygwin cmd to use from windows
#------------------------------------------------------------------------------
def get_cygwin_command(sh, escape, cmd, args, env = None):
    '''
       get_cygwin_command

       Get the cygwin cmd to use from windows
    '''
    cygwin_env = {}
   
    if env:
        for key in env.keys():
            cygwin_env[escape(key)] = str(escape(env[key]))

    #import pdb; pdb.set_trace()
    # zeroth element is always the command make sure it is POSIX
    args[0] = str(args[0]).replace('\\', '/')
   
    args_str = string.join(args)
    args_esc = "\"" + escape(args_str) + "\""
    cmdline = string.join([sh, '-c ', args_esc])
    #import pdb; pdb.set_trace()      
    #print cmdline
    return cmdline, cygwin_env

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('cygwin_config')

def generate(env):
    '''
       generate
    '''
    #---------------------------------------------------------------------------
    # Tool path definitions (may be tool set dependent - ADS vs RVCT)
    #---------------------------------------------------------------------------
    if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
        if os.environ.has_key('GNUPATH'):
            env.Replace(GNUPATH = os.environ['GNUPATH'])
            env.Replace(GNU_INCPATH = env.RealPath("${GNUPATH}/../usr/include"))
        else:
            print ("---------------------------------------------------------" \
                "----------------------")
            print ("*** GNUPATH is not defined on this environment")
            print ("*** Please run [\\\\stone\\aswcrm\\smart\\NT\\bin\\" \
                "RVCT221P593gnumake.cmd]")
            print ("*** to setup GNU environment.")
            print ("---------------------------------------------------------" \
                "----------------------")
            raise RuntimeError, "GNUPATH is not defined on this environment"
            exit(1)
    else:
        env.Replace(GNU_INCPATH = env.RealPath("/usr/include"))
      
    if env.has_key('ENV'):
        env['ENV']['PATH'] = env['ENV']['PATH'] + env.subst(";${GNUPATH}")
        #print env.Dump()

    env['PLATFORM']       = 'cygwin'
    #env['ENV']['PATH']    = '/usr/local/bin:/opt/bin:/bin:/usr/bin'
    env['CYGWIN_NULL']    = ''
    env['OBJPREFIX']      = ''
    env['OBJSUFFIX']      = '.o'
    env['SHOBJPREFIX']    = '$OBJPREFIX'
    env['SHOBJSUFFIX']    = '$OBJSUFFIX'
    env['PROGPREFIX']     = ''
    env['PROGSUFFIX']     = ''
    env['LIBPREFIX']      = 'lib'
    env['LIBSUFFIX']      = '.a'
    env['SHLIBPREFIX']    = '$LIBPREFIX'
    env['SHLIBSUFFIX']    = '.so'
    env['LIBPREFIXES']    = [ '$LIBPREFIX' ]
    env['LIBSUFFIXES']    = [ '$LIBSUFFIX', '$SHLIBSUFFIX' ]
    env['ESCAPE']         = escape
    env['GET_CYGWIN_CMD'] = get_cygwin_command
    env['SHELL']          = 'sh'
   
    if env.PathExists("${GNUPATH}\\sh.exe"):
        env['SHELL'] = env.subst('${GNUPATH}\\sh')
    else:
        env.PrintWarning \
            ("${GNUPATH}\\sh.exe not found using system environment.")
   
    # Ecapsulate within quotes so we retain the space after --via
    env.Replace(TEMPFILEPREFIX = "@")
   
    # Repalce RDirs for a Posix compliant version
    env.Replace(PosixRDirs = posix_RDirs)
    env.Replace(_CPPINCFLAGS = '$( ${_concat(INCPREFIX, CPPPATH, INCSUFFIX, ' \
        '__env__, PosixRDirs, TARGET, SOURCE)} $)')
    env.Replace(_LIBDIRFLAGS = '$( ${_concat(LIBDIRPREFIX, LIBPATH, ' \
        'LIBDIRSUFFIX, __env__, PosixRDirs, TARGET, SOURCE)} $)')
    #env.Replace(_LIBFLAGS = '${_stripixes(LIBLINKPREFIX, LIBS, ' \
    #    'LIBLINKSUFFIX, LIBPREFIX, LIBSUFFIX, __env__)}')
    env.Replace(_LIBFLAGS_POSIX = '$( ${_concat(CYGWIN_NULL, _LIBFLAGS, '\
        'CYGWIN_NULL, __env__, PosixRDirs)} $)')
   


    env.InitTempfile()
    env.InitSpawn()
