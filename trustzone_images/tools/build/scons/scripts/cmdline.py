#===============================================================================
#
# CoreBSP CMD Line parser rules
#
# GENERAL DESCRIPTION
#    Command line rules script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/cmdline.py#1 $
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
# 05/15/09   wd      Create
#
#===============================================================================
import os
import sys
import SCons
from SCons.Script import *

def validator(key, value, environment):
   if value == None:
      m = 'option %s does not exist must be defined.'
      raise SCons.Errors.UserError(m % key)

#------------------------------------------------------------------------------
# Hooks for SCons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('cmdline')

def generate(env):
   #=============================================================================
   # Add options to SCons
   #=============================================================================
   # add verbose option
   SCons.Script.AddOption('--verbose',
           dest='verbose',
           type='int',
           nargs=1,
           default='0',
           help='Verbose level 3=raw (no formatting), 2=detailed, 1=limited, 0=off')

   #verbose = env.GetOption('verbose')
   #print "Found verbose level "+ str(verbose)

   # add progress option
   SCons.Script.AddOption('--progress',
           dest='progress',
           action="store_true",
           default=False,
           help='Display build progress')

   # add buildspec option
   SCons.Script.AddOption('--buildspec',
           dest='buildspec',
           type="string",
           default='none',
           help='Create build spec file of all commands (full log output)')

   # add serial-stdout option
   SCons.Script.AddOption('--deserial-stdout',
           dest='serial_stdout',
           action="store_false",
           default=True,
           help='Print the output line-at-a-time instead of waiting for the process to finish (better debugging)')

   # add debugincpaths option
   SCons.Script.AddOption('--debugincpaths',
           dest='debugincpaths',
           type='string',
           default='none',
           help='Display incpath scanning debug information (all, int[ternal], ext[ternal])')

   # add preprocess option
   SCons.Script.AddOption('--preprocess',
           dest='preprocess',
           action="store_true",
           default=False,
           help='Generate C-preprocess file *.o.pp for any compiled file')
           
   # add mixasm option
   SCons.Script.AddOption('--mixasm',
           dest='mixasm',
           action="store_true",
           default=False,
           help='Generate mixed assembly/source file *.o.mix for any compiled file')

   # add hdeptree option
   SCons.Script.AddOption('--hdeptree',
           dest='hdeptree',
           action="store_true",
           default=False,
           help='Generate header dependency tree file *.dep for any compiled file')
           
   # add useincfiles option
   SCons.Script.AddOption('--useincfiles',
           dest='useincfiles',
           action="store_true",
           default=False,
           help='use inc files with --via instead of command line -I directives')

   # add listtargets option
   SCons.Script.AddOption('--listtargets',
           dest='listtargets',
           action="store_true",
           default=False,
           help='list of build targets found')
           
   # add listtargets option
   SCons.Script.AddOption('--listapis',
           dest='listapis',
           action="store_true",
           default=False,
           help='list of Public and AU Restricted APIs found')
           
   # add frommake option (internal use)
   SCons.Script.AddOption('--frommake',
           dest='frommake',
           action="store_true",
           default=False,
           help='Used when calling SCons from within make')

   # add frommake option (internal use)
   SCons.Script.AddOption('--debuginfo',
           dest='debuginfo',
           type='string',
           default='none',
           help='Display debug information (incpaths-int,incpaths-ext,scripts)')

   # add frommake option (internal use)
   SCons.Script.AddOption('--filter',
           dest='filter',
           type='string',
           default='none',
           help='filter, a way to reduce the files being built to a specify group.')

   # add force build/rebuild
   SCons.Script.AddOption('--force-build',
           dest='force_build',
           action="store_true",
           default=False,
           help='Force build/rebuild (delete cache database)')

   # add linkonly option (experimental...)
   SCons.Script.AddOption('--callgraph',
           dest='callgraph',
           action="store_true",
           default=False,
           help='Create call graphs on supported linkers only.')

   # add linkonly option (experimental...)
   SCons.Script.AddOption('--linkonly',
           dest='linkonly',
           action="store_true",
           default=False,
           help='Do not build libraries link only')

   # add target config file option
   SCons.Script.AddOption('--tcfgf',
           dest='tcfgf',
           type="string",
           default='target.builds',
           help='Use builds file supply by user')

   # add cleanpack option
   SCons.Script.AddOption('--cleanpack',
           dest='cleanpack',
           action="store_true",
           default=False,
           help='Perform special clean processing for Pack build')

   #=============================================================================
   # Set commands and extention string
   #=============================================================================
   
   # select correct python command depending if ath is defined in environment
   pythoncmd = "python"
   pythonpath = os.environ.get('PYTHON_PATH')
   
   if pythonpath:
      pythoncmd = env.RealPath("".join([pythonpath, '/', pythoncmd]))
  
   env.Replace(PYTHONCMD = pythoncmd)
  
   # define executabel extentions
   if sys.platform.startswith("linux"):
      PLATFORM = 'linux'
      EXE_EXT = ''
   else:
      PLATFORM = 'windows'
      EXE_EXT = '.exe'

   env.Replace(PLATFORM = PLATFORM)
   env.Replace(EXE_EXT = EXE_EXT)

   #=============================================================================
   # Define root paths, etc
   #=============================================================================
   BUILD_ROOT = env.RealPath('${BUILD_ROOT}', posix=True)
   TARGET_ROOT = env.RealPath('${TARGET_ROOT}', posix=True)
   BUILD_MS_ROOT = env.RealPath('${BUILD_ROOT}/build/ms', posix=True)
   COREBSP_ROOT = env.RealPath('${BUILD_ROOT}/core', posix=True)
   INC_ROOT = env.RealPath('${BUILD_ROOT}', posix=True)

   L4_ROOT = env.RealPath('${BUILD_ROOT}/core/kernel/l4', posix=True)
   if not os.path.exists(L4_ROOT):
      L4_ROOT = env.RealPath('${BUILD_ROOT}/core/kernel', posix=True)
   if not os.path.exists(L4_ROOT):
      L4_ROOT = env.RealPath('${BUILD_ROOT}/../../platform/l4', posix=True)

   PLAT_ROOT = env.RealPath("${BUILD_ROOT}/../../platform", posix=True)
   if not os.path.exists(PLAT_ROOT):
      PLAT_ROOT = env.RealPath("${BUILD_ROOT}/platform", posix=True)
   if not os.path.exists(PLAT_ROOT):
      PLAT_ROOT = env.RealPath("${BUILD_ROOT}/brewmp", posix=True)      
   
   if 'BUILD_SCRIPTS_ROOT' not in env:
      BUILD_SCRIPTS_ROOT = env.RealPath('${BUILD_ROOT}/tools/build/scons/scripts', posix=True)
      env.Replace(BUILD_SCRIPTS_ROOT = BUILD_SCRIPTS_ROOT)

   if 'BUILD_SCRIPTS_OEM_ROOT' not in env:
      BUILD_SCRIPTS_OEM_ROOT = env.RealPath('${BUILD_ROOT}/tools/build/scons/scripts', posix=True)
      env.Replace(BUILD_SCRIPTS_OEM_ROOT = BUILD_SCRIPTS_OEM_ROOT)

   if 'BUILD_SCRIPTS_TCFG_ROOT' not in env:
      BUILD_SCRIPTS_TCFG_ROOT = env.RealPath('${BUILD_ROOT}/tools/build/scons/tcfg', posix=True)
      env.Replace(BUILD_SCRIPTS_TCFG_ROOT = BUILD_SCRIPTS_TCFG_ROOT)

         #          1         2         3         4         5         6         7         8
         #012345678901234567890123456789012345678901234567890123456789012345678901234567890
   #print "PLATFORM      = %s " % (PLATFORM)
   print "PYTHON Ver    = %s " % sys.version
   print "SCONS Ver     = %s " % SCons.__version__ 
   print "SCONS QAPI    = %s " % env.get('SCONS_QAPI', 'unknown')
   
   if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:   
      print "OS ENV SIZE   = %d " % len(str(os.environ)) 
   
   print "TARGET_ROOT   = %s " % (TARGET_ROOT)
   print "BUILD_ROOT    = %s " % (BUILD_ROOT)
   print "BUILD_MS_ROOT = %s " % (BUILD_MS_ROOT)

   #=============================================================================
   # Define build variables
   #=============================================================================
   # define variables
   vars = Variables('build_parms.py')
   
   # add variebales that can be cached in params data file
   vars.AddVariables(
      ('BUILD_ID', 'Target Build ID from AMSS world i.e. BUILD_ID=SKCAAAF', None, validator),
      ('BUILD_VER', 'Target Build Version i.e. BUILD_VER=1109', '0000', validator),
      ('BUILD_ASIC', 'Target Build ASIC i.e. BUILD_ASIC=8200A', None, validator),
      ('MSM_ID', 'Target MSM ID i.e. MSM_ID=76XXT', 'none', validator),
      ('HAL_PLATFORM', 'Target HAL platform i.e. HAL_PLATFORM=7627', None, validator),
      ('TARGET_FAMILY', 'Target family i.e. TARGET_FAMILY=7600', None, validator),
      ('CHIPSET', 'Full target chipset i.e. CHIPSET=msm7x30', None, validator),
   )

   # update environment and read command line variables
   vars.Update(env)
   vars.Save('build_parms.py', env)
   
   # these variebales should not be cached
   vars.AddVariables(
      ('filter', 'filter what subsystem and/or unit and/or subunit to build with a path like syntax e.i. core/storage/flash/dal', None),
      ('CFLAGS', 'Adds CFLAGS to compiler command line', None),
      ('LFLAGS', 'Adds LINK FLAGS to linker command line', None),
      ('USES_FLAGS', 'Adds USES_FLAGS to the environment USES_FLAGS=USEX_X,USES_Y,USES_Z', None),
   )   
   
   # update environment and read command line variables
   vars.Update(env)

   # Need to be after initial variables because of dependencies
   vars.AddVariables(
      SCons.Script.PathVariable(
         'LIB_ROOT',
         'The root path where libaries will be installed',
         BUILD_ROOT +'/build/ms/libs/'+ env['BUILD_ID'], PathVariable.PathAccept),

      SCons.Script.PathVariable(
         'MBN_ROOT',
         'The root path where MBNs will be installed',
         BUILD_ROOT +'/build/ms/bin/'+ env['BUILD_ID'], PathVariable.PathAccept),
   )

   # update environment and read command line variables
   vars.Update(env)
   
   env.Replace(MSMID = env['MSM_ID'])
   
   #=============================================================================

   CUST_H = str("cust" + env['BUILD_ID'] + ".h").lower()

   EXTERNAL_INC = SCons.Script.ARGUMENTS.get('EXTERNAL_INC')
   if not EXTERNAL_INC:
      EXTERNAL_INC = 'external.inc'

   #=============================================================================
   # Add Other variables to environment
   #=============================================================================

   env.Replace(BUILD_ROOT = BUILD_ROOT)
   env.Replace(BUILD_MS_ROOT = BUILD_MS_ROOT)
   env.Replace(COREBSP_ROOT = COREBSP_ROOT)
   env.Replace(L4_ROOT = L4_ROOT)
   env.Replace(INC_ROOT = INC_ROOT)
   env.Replace(EXTERNAL_INC = EXTERNAL_INC)
   env.Replace(PLAT_ROOT = PLAT_ROOT)
   env.Replace(TARGET_ROOT = TARGET_ROOT)

   env.Replace(PROC = 'uknown')
   env.Replace(INSTALL_LIBPATH = env['LIB_ROOT'])
   env.Replace(CUST_H = CUST_H)
   env.Replace(BUILDPATH = '')
   
   env.Replace(MBN_ROOT = env.RealPath("${MBN_ROOT}", posix=True))

   #=============================================================================
   # Print current configuration
   #=============================================================================
         #          1         2         3         4         5         6         7         8
         #012345678901234567890123456789012345678901234567890123456789012345678901234567890
   print "MBN_ROOT      = %s" % env['MBN_ROOT']
   print "BUILD_ID      = %s" % env['BUILD_ID']
   print "BUILD_VER     = %s" % env['BUILD_VER']
   print "BUILD_ASIC    = %s" % env['BUILD_ASIC']
   print "MSM_ID        = %s" % env['MSM_ID']
   print "CHIPSET       = %s" % env['CHIPSET']
   print "TARGET_FAMILY = %s" % env['TARGET_FAMILY']
   print "HAL_PLATFORM  = %s" % env['HAL_PLATFORM']
   print " "
   print "COMMAND_LINE_TARGETS are", map(str, SCons.Script.COMMAND_LINE_TARGETS)
   print "BUILD_TARGETS are       ", map(str, BUILD_TARGETS)

   #=============================================================================
   # Decode filter / subsystem,unit,subunit params
   #=============================================================================
   subsystem = ARGUMENTS.get('subsystem')
   unit = ARGUMENTS.get('unit')
   subunit = ARGUMENTS.get('subunit')
   
   if (subsystem is not None) or (unit is not None) or (subunit is not None):
      raise RuntimeError, "subunit, unit, subunit are obsolete, use filter instead."
   
   filter = str(env.GetOption('filter'))
   if filter == 'none':
      filter = ARGUMENTS.get('filter')   
      
   FILTER_SEP = '/'
      
   env.Replace(FILTER_SEP = FILTER_SEP)
      
   if filter is not None:
      env.Replace(FILTER_OPT = filter.split(','))
   else:
      env.Replace(FILTER_OPT = filter)
      
   #=============================================================================
   # Decode debug info params
   #=============================================================================
   debugincpaths = str(env.GetOption('debugincpaths'))

   if debugincpaths != 'none':
      raise RuntimeError, "--debugincpaths is obsolete, use --debuginfo instead."
      
   debuginfo = str(env.GetOption('debuginfo'))
   debuginfo = debuginfo.lower()
   debuginfo_opt = debuginfo.split(',')
   debuginfo_opt.append('any')
   env.Replace(DEBUGINFO_OPT = debuginfo_opt)
   
   #=============================================================================
   # Validate cleanpack option
   #=============================================================================
   cleanpack = env.GetOption('cleanpack')
   if cleanpack:
      clean = env.GetOption('clean')
      if not clean:
         env.PrintError ("-------------------------------------------------------------------------------")
         env.PrintError ("MUST have '--clean' enabled to use CleanPack option '--cleanpack'")
         env.PrintError ("-------------------------------------------------------------------------------")
         Exit(1)
      else:
         env.PrintWarning ("-------------------------------------------------------------------------------")
         env.PrintWarning ("CleanPack operation is irreversible!!")
         env.PrintWarning ("-------------------------------------------------------------------------------")
      
   #=============================================================================
   # Add exception to USES_FLAGS which is already used internally
   #=============================================================================
   env.Replace(USES_FLAGS_CL = env.get('USES_FLAGS'))
   
   #=============================================================================
   # add help strings to environment
   #=============================================================================

   env.Replace(VARS_HELP = vars.GenerateHelpText(env))
   env.Replace(OPT_HELP =  """
Local Options:
  -h, --help                  Print defined help message (this help).
  -H, --help-options          SCons help only, skip local options.
  -c, --clean, --remove       Remove specified targets and dependencies.
  -k, --keep-going            Keep going when a target can't be made.  
  -n, --no-exec, --just-print, --dry-run, --recon
                              Don't build; just print commands.
  --force-build               Force build/rebuild, !!WARNING deletes scons cache data.
  --filter=[<au_name>[/<subsystem>[/<unit>[/<subunit>]]]],[]....
                              Forces SCons to only load specified scripts, 
                              multiple filters can be used separated by a
                              comma (,).
                                 au_name               build only a area unit
                                 au_name/subsystem     build only a subsystem
                                 au_name/subunit/unit  build only a unit
                                 
                              examples:
                                 core
                                 core/debugtools
                                 core/debugtools/tmc
                                 
                              note: Filter assumes building parts from a target 
                              i.e. libraries, objfiles, etc. therefore any link 
                              and post link steps are disabled.
  
  --verbose=<level>           Verbose level 
                                 3=raw (no formatting), 
                                 2=detailed (source, target, cmd-line),
                                 1=limited, 
                                 0=off.
  --debuginfo=<opt1,opt2,...> Debug Information, options are:
                                 scripts,  Loading/Reading scripts info
                                 incpaths, Path Info per call to Require<*>Api()
                                 <others>, User added options...

                                 ** NOTE: incpath info output can be massive, 
                                    better use with --filter. See filter usage.
                                    
  --preprocess                Generates "C" pre-process files (*.o.pp) for any 
                                 .c or .cpp file compiled.
  --mixasm                    Generates assembly/source files (*.o.mix) for any 
                                 .c or .cpp files compiled for ARM other compiler
                                 may generate assembly only.
  --hdeptree                  Generates header dependecy tree (*.o.dep) for any 
                                 .c or .cpp file compiled.
  --progress                  Display build progress.
  --listtargets               Display a list of build-able targets.
  --listapis                  Display a list of Public and Restricted APIs.
  --tcfgf=<builds file>       Provide *.builds file as input.
  --cleanpack                 Can only be used in combination with clean option,
                              IMPORTANT: This operation cannot be reversed, all marked
                              pack files will be removed/deleted/erased.

  Usage:
      build <target> <options>
      build <build path/file.o> <options>
      build <build path/libfile.lib> <options>

  Where:
      target                  amss, amsslibs, coreimg, arm9, arm11, all, etc.
                                 use build -h or build --listtargets to see 
                                 the list available target and corresponding 
                                 aliases.
      path/to/file.o          build only file.o
      path/to/libfile.lib     build only libfile.lib (.lib win, .a posix)

  Examples:
      'build all'             to build all targets and dependencies.
      'build arm9'            to build arm9 image and dependencies 
      'build amss_modem --filter=core/services'
                              to build services subsystem for arm9
                              and dependencies.
      'build arm9 --filter=core/debugtools/tmc,core/services'
                              to build tmc from debugtools and services subsystem 
                              for arm9 target and dependencies.
      'build ../../services/diag/build/cbsp/arm9/diag.lib'
                              to build diag.lib for CBSP arm9 and dependencies.
      'build ../../services/diag/build/cbsp/arm9/diaglog.o'
                              to build diaglog.o for CBSP arm9.
      'build all --verbose=2' to build all targets displaying the command
                              used to compile each files.
      'build all --progress'  to build all targets and displaying SCons progress
                              useful for incremental builds, shows what is
                              SCons is doing.
      'build boot --tcfgf=8960.target.builds'
                              to build boot using 8960.target.builds file
                              
      NOTE: arm9 depends on target supported CPUs, use build -h or 
            build --listtargets to see the list of available target and 
            corresponding aliases.
            
      TIP:  To find-out what targets get build with an alias use use build -h or 
            build --listtargets with grep. 
            To list the targets that get build when using the alias "amss_modem"
            For example:
               build -h | grep amss_modem
   """)

   env.Help(env['OPT_HELP'] + env['VARS_HELP'])


#          1         2         3         4         5         6         7         8
#012345678901234567890123456789012345678901234567890123456789012345678901234567890

