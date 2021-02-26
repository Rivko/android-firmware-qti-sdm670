#===============================================================================
#
# Cygwin helpers
#
# GENERAL DESCRIPTION
#    Common utilities used across the build system
#
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/scripts/cygwin.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/15/09   wd      Create
#
#===============================================================================
import os
import string

#------------------------------------------------------------------------------
# Posix RDirs
# Makes sure the inc paths are posix compliant.
#------------------------------------------------------------------------------
def posix_RDirs (path_list):
   path_list = [str(path).replace('\\', '/') for path in path_list]
   return path_list

#------------------------------------------------------------------------------
# Escape
# Unescapes escapes chars for passing to sh -c "cmd args"
#------------------------------------------------------------------------------
def escape(arg):
   "escape shell special characters"
   slash = '\\'
   special = '"$()'

   arg = string.replace(arg, slash, slash+slash)
   for c in special:
     arg = string.replace(arg, c, slash+c)

   #return '"' + arg + '"'
   return arg

#------------------------------------------------------------------------------
# get_cygwin_command
# Get the cygwin cmd to use from windows
#------------------------------------------------------------------------------
def get_cygwin_command(sh, escape, cmd, args, env = None):
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
   return env.Detect('cygwin_config')

def generate(env):
   #-------------------------------------------------------------------------------
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #-------------------------------------------------------------------------------
   if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
      if os.environ.has_key('GNUPATH'):
         env.Replace(GNUPATH = os.environ['GNUPATH'])
         env.Replace(GNU_INCPATH = env.RealPath("${GNUPATH}/../usr/include"))
      else:
         print ("-------------------------------------------------------------------------------")
         print ("*** GNUPATH is not defined on this environment")
         print ("*** Please run [\\\\stone\\aswcrm\\smart\\NT\\bin\\RVCT221P593gnumake.cmd]")
         print ("*** to setup GNU environment.")
         print ("-------------------------------------------------------------------------------")
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
      env.PrintWarning("${GNUPATH}\\sh.exe not found using system environment.")
   
   # Ecapsulate within quotes so we retain the space after --via
   env.Replace(TEMPFILEPREFIX = "@")
   
   # Repalce RDirs for a Posix compliant version
   env.Replace(PosixRDirs = posix_RDirs)
   env.Replace(_CPPINCFLAGS = '$( ${_concat(INCPREFIX, CPPPATH, INCSUFFIX, __env__, PosixRDirs, TARGET, SOURCE)} $)')
   env.Replace(_LIBDIRFLAGS = '$( ${_concat(LIBDIRPREFIX, LIBPATH, LIBDIRSUFFIX, __env__, PosixRDirs, TARGET, SOURCE)} $)')
   #env.Replace(_LIBFLAGS = '${_stripixes(LIBLINKPREFIX, LIBS, LIBLINKSUFFIX, LIBPREFIX, LIBSUFFIX, __env__)}')
   env.Replace(_LIBFLAGS_POSIX = '$( ${_concat(CYGWIN_NULL, _LIBFLAGS, CYGWIN_NULL, __env__, PosixRDirs)} $)')
   


   env.InitTempfile()
   env.InitSpawn()
