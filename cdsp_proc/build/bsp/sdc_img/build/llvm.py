#===============================================================================
#
# ARM tool rules
#
# GENERAL DESCRIPTION
#    LLVM 3.8 Tools and greater Tools definitions
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0.c3/bsp/sdc_img/build/llvm.py#1 $
#  $DateTime 2016/06/17 15:44:17 $
#  $Change: 16823152 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#01/28/14  ygui    add new feature of switch 32bit or 64bit compiler conditionally
#03/04/16  pdosi   added support for QCLD and 3.7 LLVM.
#06/17/16  hongy   added support for QCLD and 3.8 LLVM.
#===============================================================================
import os
import string
import SCons.Action
from SCons.Script import *

# Assembly compile flags (note first pass is through armcc using -E option then passed to armas, see
# build rule below
# standard compile flags
build_dflags = '-DCUST_H=\\"${CUST_H}\\"' \
      ' -DBUILD_TARGET=\\"${BUILD_ID}\\" -DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\"'
#------------------------------------------------------------------------------
# RDirs
# Makes sure the inc paths are as provided, don't let scons mess with them.
#------------------------------------------------------------------------------
def RDirs (path_list):
   #path_list = [str(path).replace('\\', '/') for path in path_list]
   return path_list

#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()

   # call defualt static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)

   preprocess = env.GetOption('preprocess')
   mixasm = env.GetOption('mixasm')
   hdeptree = env.GetOption('hdeptree')

   if preprocess or hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
      env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))

   if mixasm:
      env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))

   if hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))


   if 'EXPLICIT_DEPS' in env:
      env.Depends(tgt, env['EXPLICIT_DEPS'])

   return (tgt, src)

#-------------------------------------------------------------------------------
# Add Program (Link into exe, elf, etc).
#-------------------------------------------------------------------------------
def add_program(env, target=None, source=None, **kargs):
   '''
      add_program

      Add Program (Link into exe, elf, etc).
   '''

   #---------------------------------------------------------------------------
   # Init vars and environment needed to use Program builder
   image_env = env.get('IMAGE_ENV')

   # variables for Program builder
   if image_env is not None:
      if target is None:
         target = image_env.get('TARGET_NAME')

      if source is None:
         objs = image_env.get('IMAGE_OBJS')
      else:
         objs = source
      objs = env.Flatten([objs])

      if 'LIBS' not in kargs:
         libs = image_env.get('IMAGE_LIBS')
         #kargs['LIBS'] = image_env.get('IMAGE_LIBS')
      else:
         libs = kargs['LIBS']
      libs = env.Flatten([libs])

      del kargs['LIBS']

      if 'SHARED_LIBS' in kargs:
         sh_libs = kargs['SHARED_LIBS']
      else:
        sh_libs = []
      if 'SHARED_LIBS_DYNSYM' in kargs:
         sh_libs_dynsym = env.Flatten([kargs['SHARED_LIBS_DYNSYM']])
      else:
         sh_libs_dynsym = []
      if 'SHARED_LIBS_EXTSYM' in kargs:
         sh_libs_extsym = env.Flatten([kargs['SHARED_LIBS_EXTSYM']])
      else:
         sh_libs_extsym = []
   # list file extraction
   list_files = []
   if 'LISTFILES' in kargs:
      list_files = kargs.get('LISTFILES')
      if list_files is None:
         pass
      elif type(list_files) is not list:
         list_files = [list_files]
   elif image_env is not None:
      list_files = image_env.get('IMAGE_LISTFILES')

   #---------------------------------------------------------------------------
   # Ready for Program builder...
   target_elf = env.Program(target, source=[], **kargs)

   # get target base name
   target_base, target_ext = os.path.splitext(target_elf[0].get_abspath())
   build_dir = env.Dir('#').get_abspath()

   #import pdb;pdb.set_trace()

   # create list of files used during link.
   if len(objs) > 0:
      objs_lf_fname = env.subst("%s_objs.lf" % target_base)
      objs_linker_file = env.ListFileBuilder(objs_lf_fname, objs, \
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, objs_linker_file)
      env.Append(LISTFILES=[objs_linker_file[0].get_abspath()])

   if len(libs) > 0:
      libs_lf_fname = env.subst("%s_libs.lf" % target_base)
      libs_linker_file = env.ListFileBuilder(libs_lf_fname, libs, \
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, libs_linker_file)
      env.Append(LISTFILES=[libs_linker_file[0].get_abspath()])
   if len(sh_libs) > 0 and not \
      (len(sh_libs_dynsym) > 0 or len(sh_libs_extsym) > 0):
      env.LoadToolScript('${BUILD_SCRIPTS_ROOT}/sharedlib_symbol.py', \
         toolpath=['${BUILD_SCRIPTS_ROOT}'])
      sh_libs_lf_fname = env.subst("%s_shared_libs.lf" % target_base)
      sh_libs_linker_file = env.ListFileBuilder(sh_libs_lf_fname, sh_libs, \
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, sh_libs_linker_file)
      sh_libs_dynsyms_fname = \
         env.subst("%s_shared_libs_dynsyms.lst" % target_base)
      sh_libs_externs_fname = \
        env.subst("%s_shared_libs_externs.lst" % target_base)
      sh_lib_link_files = env.SharedLibGenerateLinkerList(
         [sh_libs_dynsyms_fname, sh_libs_externs_fname], sh_libs)
      env.Depends(target_elf, sh_lib_link_files)
      #FIXME - is this valid for LLVM? hexagon is slightly different
      env.Append(LINKFLAGSH=" --edit=%s" % (sh_libs_dynsyms_fname))
#      env.Append(LINKFLAGS="--dynamic-list=%s -T%s " % (sh_libs_dynsyms_fname, sh_libs_extern$
   elif len(sh_libs) > 0:
      env.PrintError("")
      env.PrintError("-----------------------------------------------------" \
         "--------------------------")
      env.PrintError("*** env.AddProgram():")
      env.PrintError("*** Cannot use SHARED_LIBS and SHARED_LIBS_DYNSYM or " \
         "SHARED_LIBS_EXTSYM")
      env.PrintError("*** concurrently")
      env.PrintError('-----------------------------------------------------' \
         '--------------------------')
      env.PrintError("")
      Exit(1)
   if len(sh_libs_dynsym) > 0:
      #FIXME - is this valid for LLVM? hexagon is slightly different
      env.Append(LINKFLAGS="--edit=%s " % \
#      env.Append(LINKFLAGS="--dynamic-list=%s " % \
         env.RealPath(str(sh_libs_dynsym[0]), posix=True))
      env.Depends(target_elf, sh_libs_dynsym)
   if len(sh_libs_extsym) > 0:
      env.Append(LINKFLAGS="-T%s " % \
      env.RealPath(str(sh_libs_extsym[0]), posix=True))
      env.Depends(target_elf, sh_libs_extsym)

#   if 'USES_ANALYTIC_DATA' in env and len(objs) > 0 and len(libs) > 0:
   if 'USES_ANALYTIC_DATA' in env and (len(objs) > 0 or len(libs) > 0):
      analytic_fname = env.subst("%s_analytic.zip" % target_base)
      analytic_file = env.AnalyticBuilder(analytic_fname, [objs, libs], \
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, analytic_file)
   # add dependency on external build system files
   exbldsys_files = None
   if len(list_files) > 0:
      """
      # external list files don't have a header or any other information that
      # gets updated if any of the files listed have changed. Since our
      # general SCcons decider relies on time-stamp then MD5 and since the
      # content of these list files have not changed even if the actual
      # libraries objects listed have changed (file names or the number of
      # files listed have not changed but MD5 of the files listed might have
      # changed), we need to change the decider for this builder to rely
      # solemnly on time-stamp as the decider.
      exbldsys_env = env.Clone()
      exbldsys_env.Decider('timestamp-newer')
      """
      exbldsys_list_fname = env.subst("%s_exbldsys.lf" % target_base)
      exbldsys_files = env.ListFileExtractor(exbldsys_list_fname, list_files)
      #exbldsys_files = exbldsys_env.ListFileExtractor(exbldsys_list_fname, list_files)
      env.Depends(target_elf, exbldsys_files)
      env.Append(LISTFILES=[exbldsys_files[0].path])

   """
   # other special dependencies for ELF come
   # from the special QDSP5OBJ link groups
   for obj_group_num in range(1, 5):
      obj_group = env.GetHexagonLinkGroup(obj_group_num)
      for i in obj_group:
         env.Depends(target_elf, env.RealPath(i))
   """

   # side effects of creating a program...
   #import pdb;pdb.set_trace()
   map_file = env.subst("%s.map" % target_base)

   env.SideEffect(map_file, target_elf)
   env.Clean(target_elf, map_file)

   sym_file = env.subst("%s.sym" % target_base)
   env.SideEffect(sym_file, target_elf)
   env.Clean(target_elf, sym_file)

   callg_file = env.subst("%s.htm"  % target_base)
   env.SideEffect(callg_file, target_elf)
   env.Clean(target_elf, callg_file)

   return target_elf

#-------------------------------------------------------------------------------
# Add Shared Library:
#-------------------------------------------------------------------------------
def add_shared_library(env, targets, library, sources, \
   add_to_list=True, **args):
   '''
      add_shared_library

      Add Shared Library
   '''
   library_node = None
   #import pdb;pdb.set_trace()
   if env.IsKeyEnable(targets):
      #library_node_i = env.SharedLibrary(library, sources)
      #library_node = env.AddArtifact(targets, library_node_i)

      library_node = env.SharedLibrary(library, sources)

      if add_to_list:
         # now actually add lib
         root_env = env.get('ROOT_ENV')

         # get list container for current AU or global
         # if an AU container doesn't exist
         sh_lib_list = root_env.get('AU_SHARED_LIBS')

         if sh_lib_list is not None:
            env.AddNodestoList(sh_lib_list, library_node)
         else:
            env.PrintError("Can't find key 'AU_SHARED_LIBS'!, shared lib " \
            "[%s] was not added to any list" % str(library_node))
            env.PrintError('')
            Exit(1)

   return library_node

#-------------------------------------------------------------------------------
# Add List File:
# API for analytic builder for use for files compiled via list files
# to also automatically capture analytics data
#-------------------------------------------------------------------------------
def add_list_file(env, target=None, source=None, **kargs):
   '''
      add_list_file

      Add List File:
      API for analytic builder for use for files compiled via list files
      to also automatically capture analytics data
   '''
   source = env.Flatten([source])
   if 'LIBS' not in kargs:
      libs = image_env.get('IMAGE_LIBS')
   else:
      libs = kargs['LIBS']
   libs = env.Flatten([libs])

   add_header_bool = True
   posix_bool = True
   """
    if 'add_header' not in kargs:
        add_header_bool = False
    else:
        add_header_bool = kargs['add_header']

    if 'relative_path' not in kargs:
        build_dir = env.Dir('#').get_abspath()
    else:
        build_dir = kargs['relative_path']

    if 'posix' not in kargs:
        posix_bool = False
    else:
        posix_bool = kargs['posix']
  """

   build_dir = env.Dir('#').get_abspath()
   list_file = env.ListFileBuilder(target, [source, libs], \
     add_header=add_header_bool, relative_path=build_dir, posix=posix_bool)

   if 'USES_ANALYTIC_DATA' in env and (len(source) > 0 or len(libs) > 0):
      target_base, target_ext = os.path.splitext(list_file[0].get_abspath())
      analytic_fname = env.subst("%s_analytic.zip" % target_base)
      analytic_file = env.AnalyticBuilder(analytic_fname, [source, libs], \
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(list_file, analytic_file)
   #return list_file

#------------------------------------------------------------------------------
# Chain emitter for asm files
# Adds explicit dep
#------------------------------------------------------------------------------
def S_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()

   # call defualt static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)

   # add cleaners for assembling side-effects, taht scons can't auto detect
   env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
   env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
   env.Clean(tgt, "".join([str(tgt[0]), ".lst"]))

   if 'EXPLICIT_DEPS' in env:
      env.Depends(tgt, env['EXPLICIT_DEPS'])

   return (tgt, src)

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('arm')

def generate(env):
   #============================================================================
   # Special init for our tools and special dependency in CUST_H
   #============================================================================
   #env.Replace(ARM_TOOLS_ENV=env)

   env.AddMethod(add_program, "AddProgram")
   env.AddMethod(add_shared_library, "AddSharedLibrary")

   env.AddMethod(add_list_file, "AddListFile")

   # Replace Static Object Emitter
   env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)
   #env.Object.builder.add_emitter(suffix='.S', emitter=S_static_obj_emitter)
   env.Replace(CC_DFLAGS = build_dflags)

   # Standard include paths always searched
   env.Append(CPPPATH = [
      ".",
      "${INC_ROOT}/build/cust",
      "${INC_ROOT}/build/ms",
   ])

   # special dependencies in CUST_H
   cust_scanner = SCons.Scanner.C.CScanner()
   cust_path = cust_scanner.path(env)

   if 'CUST_H' in env:
      cust_h_file = env.FindFile(env['CUST_H'], env['CPPPATH'])
      if cust_h_file:
         env.Append(EXPLICIT_DEPS = [cust_h_file])
         deps = cust_scanner(cust_h_file, env, cust_path)
         if deps:
            env.Append(EXPLICIT_DEPS = deps)

   if 'AMSS_CUST_H' in env:
      cust_h_file = env.FindFile(env['AMSS_CUST_H'], env['CPPPATH'])
      if cust_h_file:
         env.Append(EXPLICIT_DEPS = [cust_h_file])
         deps = cust_scanner(cust_h_file, env, cust_path)
         if deps:
            env.Append(EXPLICIT_DEPS = deps)

   # Repalce RDirs for a don't mess with it version
   #env.Replace(RDirs = RDirs)
   #env.Replace(_CPPINCFLAGS = '$( ${_concat(INCPREFIX, CPPPATH, INCSUFFIX, __env__, RDirs, TARGET, SOURCE)} $)')


   #============================================================================
   #                             TOOL DEFINITIONS
   #============================================================================
   # The following environment variables must be defined prior to using this make
   # file: ARMHOME, ARMINC, QCTOOLS. In addition the PATH must be updated for
   # the ARM tools.


   #----------------------------------------------------------------------------
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #----------------------------------------------------------------------------

   if os.environ.has_key('LLVMBIN'):
      env.Replace(LLVMBIN = os.environ['LLVMBIN'])
   else:
      print ("-------------------------------------------------------------------------------")
      print ("*** LLVMBIN is not defined on this environment")
      print ("*** Please set up the LLVM enviroment")
      print ("-------------------------------------------------------------------------------")
      raise RuntimeError, "LLVM is not defined on this environment"
      #exit(1)
   if os.environ.has_key('LLVMTOOLS'):
     env.Replace(LLVMTOOLS = os.environ['LLVMTOOLS'])
     LLVMTOOLS = env.RealPath("${LLVMTOOLS}", posix=False)
   else:
     raise RuntimeError, "LLVMTOOLS is not defined on this environment"
   env.PrintInfo('LLVMBIN   = ${LLVMBIN}')

   if env.PathExists("${LLVMBIN}"):
      LLVMBIN = env.RealPath("${LLVMBIN}", posix=False);
      if env['PLATFORM'] in ['windows', 'win32']:
         LLVMBIN = LLVMBIN + "\\"
      else:
         LLVMBIN = LLVMBIN + "/"
   else:
     raise RuntimeError, "${LLVMBIN} not found using system environment."



   if env.PathExists("${LLVMBIN}"):
      LLVMBIN = env.RealPath("${LLVMBIN}", posix=False);
      if env['PLATFORM'] in ['windows', 'win32']:
         LLVMBIN = LLVMBIN + "\\"
      else:
         LLVMBIN = LLVMBIN + "/"
   else:
      raise RuntimeError, "${LLVMBIN} not found using system environment."
      LLVMBIN = ""
      exit(1)

   LLVMBIN = env.RealPath("${LLVMBIN}", posix=False);

   # try to find enviroment info for otehr versions
   armtools = os.environ.get('LLVMTOOLS', None)

   arm_ver = 3.8

   # Setup the ARM major version number
   env.Replace(ARMTOOLSVERSION = arm_ver)

   env.Replace(LLVMBIN = os.environ['LLVMBIN'])
   env.Replace(LLVMLIB = os.environ['LLVMLIB'])
   env.Replace(LIBC = os.environ['LIBC'])
   #env.Replace(LLVMINCLUDE64 = os.environ['LLVMINCLUDE64'])
   env.Replace(LLVMINCLUDE = os.environ['LLVMINCLUDE'])
   env.Replace(LLVMTOOLPATH = os.environ['LLVMTOOLPATH'])

   env.PrintInfo('LLVMBIN          = ${LLVMBIN}')
   env.PrintInfo('LLVMLIB          = ${LLVMLIB}')
   #env.PrintInfo('LLVMINCLUDE64          = ${LLVMINCLUDE64}')
   env.PrintInfo('LLVMINCLUDE          = ${LLVMINCLUDE}')
   env.PrintInfo('LLVMTOOLPATH          = ${LLVMTOOLPATH}')
  
   #----------------------------------------------------------------------------
   #check for LLVM and QCLD Here
   #---------------------------------------------------------------------------- 
   LinkByQCLD=True
   is37llvm=True
   llvmbin=LLVMBIN
   LinkByGNU=False

   #----------------------------------------------------------------------------
   # GNU path definitions - begin
   #----------------------------------------------------------------------------
   #if env['PLATFORM'] in ['windows', 'win32']:
      #if os.environ.has_key('GNULIBC'):
      #   env.Replace(GNULIBC = os.environ['GNULIBC'])
      #else:
      #   print ("-------------------------------------------------------------------------------")
      #   print ("*** GNULIBC is not defined on this environment")
      #   print ("*** Please set up the GNU enviroment")
      #   print ("-------------------------------------------------------------------------------")
      #   raise RuntimeError, "GNULIBC is not defined on this environment"
         #exit(1)

      #if os.environ.has_key('GNUTOOL'):
         #env.Replace(GNUTOOL = os.environ['GNUTOOL'])
      #else:
       #  print ("-------------------------------------------------------------------------------")
       #  print ("*** GNUTOOL is not defined on this environment")
       #  print ("*** Please set up the GNU enviroment")
       # print ("-------------------------------------------------------------------------------")
       #	   raise RuntimeError, "GNUTOOL is not defined on this environment"
         #exit(1)

   #if os.environ.has_key('GNUROOT'):
    #  env.Replace(GNUROOT = os.environ['GNUROOT'])
   #if os.environ.has_key('GNUARM7'):
    #  env.Replace(GNUARM7 = os.environ['GNUARM7'])
   #if os.environ.has_key('MUSLPATH'):
    #  env.Replace(MUSLPATH = os.environ['MUSLPATH'])
   #if os.environ.has_key('MUSL32PATH'):
    #  env.Replace(MUSL32PATH = os.environ['MUSL32PATH'])


   #env.PrintInfo('GNULIBC          = ${GNULIBC}')
   #env.PrintInfo('GNUTOOL          = ${GNUTOOL}')
   #env.PrintInfo('GNUROOT          = ${GNUROOT}')
   #env.PrintInfo('GNUARM7          = ${GNUARM7}')
   #env.PrintInfo('MUSLPATH          = ${MUSLPATH}')
   #env.PrintInfo('MUSL32PATH          = ${MUSL32PATH}')*/

 #  GNULIBC = env.RealPath("${GNULIBC}", posix=False);
  # GNUTOOL = env.RealPath("${GNUTOOL}", posix=False);
   #GNUROOT = env.RealPath("${GNUROOT}", posix=False);
   #GNUARM7 = env.RealPath("${GNUARM7}", posix=False);
   #MUSLPATH = env.RealPath("${MUSLPATH}", posix=False);
   #MUSLPATH = env.RealPath("${MUSL32PATH}", posix=False);

   #----------------------------------------------------------------------------
   # GNU path definitions - end
   #----------------------------------------------------------------------------

   if env['PROC'] == 'A53_64':
      do_64 = True
   else:
      do_64 = False

   #LLVMFlags = " -I" + os.environ['ARMROOT'] + "\include"
   LLVMFlags = ""
   ARM_DBG = ""
   env.Replace(ARM_DBG = "-g -gdwarf-2 ")
   if 'USES_NO_DEBUG' in env:
      env.Replace(ARM_DBG = "")
   #env.Replace(HEXTOOL = LLVMTOOLS + "/arm-elfcopy${EXE_EXT}")
   #env.Replace(HEXTOOL = "/prj/llvm-arm/home/common/build_tools/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-objcopy")
   env.Replace(HEXTOOL = '/pkg/qct/software/arm/RVDS/5.05bld169/bin/fromelf')

   UseClangAs = True

   if do_64:
      LLVMFlags += " -target aarch64-none-elf -w -x assembler -mllvm -aarch64-strict-align  -nostdinc  "
      #env.Replace(AS = ARMBIN + "armasm${EXE_EXT} -g --no_unaligned_access ")
      if UseClangAs:
         if env['PLATFORM'] in ['windows', 'win32']:
            env.Replace(AS = LLVMBIN + "clang${EXE_EXT}  -target aarch64-none-elf -x assembler -c ")
         else:
            env.Replace(AS = GNUROOT + "/bin/aarch64-none-elf-as${EXE_EXT} -c ")
      env.Replace(ARM_CPU = "8-A.64") # replaced again below
      env.Replace(TCC = LLVMBIN + "clang${EXE_EXT} " + LLVMFlags)
      env.Replace(TCPP = LLVMBIN + "clang${EXE_EXT} " + LLVMFlags)
   else:
      do_32 = True
      env.Replace(ARM_CPU = "-mcpu=cortex-m3")
      LLVMFlags += " -target armv7m-none-musleabi -w -fintegrated-as -mfloat-abi=softfp -mfpu=none -mcpu=cortex-m3 -mno-unaligned-access  "

      LLVMFlags += " -fms-extensions" #for anon union/struct
      if UseClangAs:
        env.Replace(AS = LLVMBIN + "/clang${EXE_EXT} -target armv7m-none-musleabi -fintegrated-as -mfloat-abi=softfp -mcpu=cortex-m3  -x assembler -c ")
      env.Replace(ARM_CPU = "8-A.32.no_neon") #replaced again below
      env.Replace(TCC = LLVMBIN + "/clang${EXE_EXT} -mthumb " + LLVMFlags)
      env.Replace(TCPP = LLVMBIN + "clang${EXE_EXT} -mthumb " + LLVMFlags)

   # env.Replace(TCC = ARMBIN + "clang${EXE_EXT} -mthumb " + LLVMFlags)

   # ARM SDT ARM 32-bit inst. set ANSI C compiler
   env.Replace(ARMCC = LLVMBIN + "/clang${EXE_EXT} -marm " + LLVMFlags)
   env.Replace(CC = env['TCC'])

   env.Replace(ARMCPP = LLVMBIN + "/clang${EXE_EXT} -marm " + LLVMFlags)

   env.Replace(ARMCPP = LLVMBIN + "/clang${EXE_EXT} -marm " + LLVMFlags)
   env.Replace(ARMCXX = env['ARMCPP'])
   env.Replace(CXX = env['ARMCXX'])

   # ARM SDT assembler


   # ARM SDT library archiver
   env.Replace(AR = "${LLVMTOOLPATH}/arm-ar")

   if do_64:
      env.Replace(LINK = LLVMBIN + "/ld.qcld -march aarch64  ")
   else:
      env.Replace(LINK = LLVMBIN + "/clang${EXE_EXT} -target armv7m-none-musleabi -fuse-ld=qcld")
   env.Replace(NM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/nm.py"))
   env.Replace(NM = "${PYTHONCMD} ${NM_FILE}")
   env.Append(BUILDERS = {'GetSymbol' : Builder(action='${LLVMTOOLPATH}/arm-nm $SOURCE > ${BUILD_ROOT}/build/ms/aaa.tmp && $NM -f ${BUILD_ROOT}/build/ms/aaa.tmp -o $TARGET ', suffix='.sym', src_suffix='.elf')})

   env.Replace(BINTOOL = "${LLVMTOOLPATH}/arm-elfcopy")

   # Preprocess assembly cleaner
   env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))
   env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")
   #if UseClangAs:
      #env.Append(PPASM = " -g")

   # Header dependency tree generator
   env.Replace(HDEPTREE_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))
   env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")

   #----------------------------------------------------------------------------
   # Processor architecture options
   #----------------------------------------------------------------------------
   env.Replace(ARM_ASM = "-D_ARM_ASM_")                 # ARM assembly language selection

   env.Replace(ARM="-DT_ARM -D__ARMEL__ -D__RVCT__")  # ARM processor target
   env.Replace(ARM_ARCH = "-target armv8a-arm")

   if 'PROC' in env:
      if do_64:
         env.Replace(ARM_CPU = "8-A.64")
      elif do_32:
         env.Replace(ARM_CPU = "8-A.32.no_neon")
      elif env['PROC'] in ['arm7']:
         env.Replace(ARM_CPU = "$ARM_CPU_ARM7")
      elif env['PROC'] in ['arm9']:
         env.Replace(ARM_CPU = "$ARM_CPU_ARM9")
      elif env['PROC'] in ['arm11']:
         env.Replace(ARM_CPU = "$ARM_CPU_ARM11")
      elif env['PROC'] in ['scorpion']:
         env.Replace(ARM_CPU = "$ARM_CPU_SCORPION")
      elif env['PROC'] in ['sparrow']:
         env.Replace(ARM_CPU = "$ARM_CPU_SPARROW")
      elif env['PROC'] in ['cortexA7']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_CORTEXA7")
      elif env['PROC'] in ['armv8-64']:
         env.Replace(ARM_CPU = "8-A.64")
   else:
      env.Replace(ARM_CPU = "$ARM_CPU_CMD=CPU_UKNOWN")

# TODO: ARM_CPU is defined above.  Where is the input for this?  Need
#       to switch between 64 bit and 32 bit for the same CPU.  These
#       arguments should depend on the CPU being A53/A57 for 8994 and
#       8916

   env.Replace(ARMCC_CPU = "-mcpu=cortex-a53") #TODO:translate QSP.no_neon.no_vfp -- no_unaligned_access
   #env.Replace(ARMCC_ARCH = "${ARM_ARCH} ${ARM}")
   #env.Replace(ARMCC_ARCH = "")
   env.Replace(ARMASM_CPU = "--cpu ${ARM_CPU} ${ARM} ${ARM_ASM}")
   if UseClangAs:
      env.Replace(ARMASM_CPU = "${ARM} ${ARM_ASM}")

   #----------------------------------------------------------------------------
   # NOTE:
   # If software stack checking support is enabled, FEATURE_STACK_CHECK must be
   # #defined in the cust*.h file to enable the required common services support.
   #
   # The following APCS options are obsolete in the ARM SDT 2.50 tools:
   #   PCSZ
   #   FRAMEPTR
   #   REENTRNT
   #----------------------------------------------------------------------------

   #----------------------------------------------------------------------------
   # Compiler/Linker/Tools output options
   #----------------------------------------------------------------------------
   env.Replace(ARMCC_OUTPUT_CMD = "-o")
   env.Replace(ARMCC_COMPILE_CMD = "-c")
   env.Replace(ARMCC_PREPROCESS_CMD = "-E -xc")
   env.Replace(ARMCC_ASMOUT_CMD = "-S")
   #env.Replace(ARMCC_ASM_CMD = "--asm")
   #env.Replace(ARMCC_INTERLEAVE_CMD = "--interleave")
   env.Replace(ARMAS_OUTPUT_CMD = "-o")
   #env.Replace(ARMAS_LIST_CMD = "--list")
   env.Replace(ARMAS_LIST_CMD = " ")

   env.Replace(ARMAR_LIB_REPLACE = "-r")
   env.Replace(ARMAR_LIB_QUITE = "-Dc")
   env.Replace(ARMLD_OUTPUT_CMD = "-o")
   #env.Replace(ARMLD_ELF_CMD = "--elf")
   #env.Replace(ARMLD_MAP_CMD = "--map")
   #env.Replace(ARMLD_INFO_CMD = "--info")
   #env.Replace(ARMLD_LIST_CMD = "--list")
   #env.Replace(ARMLD_SYMBOLS_CMD = "--symbols")
   #env.Replace(ARMLD_SYMDEFS_CMD = "--symdefs")
   #env.Replace(ARMLD_SCATTER_CMD = "--scatter")
   #env.Replace(ARMLD_REMOVE_CMD = "--remove")
   env.Replace(ARMLD_NOREMOVE_CMD = "--noremove")
   env.Replace(GNULD_REMOVE_CMD = "-Wl,--gc-sections")
   env.Replace(GNULD_NO_REMOVE_CMD = "-Wl,--no-gc-sections")
   #env.Replace(ARMLD_CALLGRAPH_CMD = "--callgraph")
#   env.Replace(ARMLD_SH_FPIC="--apcs=/fpic")
   env.Replace(ARMBIN_BIN_CMD = "-O binary")
   #env.Replace(ARMBIN_ELF_CMD = "--elf")
   env.Replace(ARMBIN_OUTPUT_CMD = " ")

   env.Replace(ARMBIN_NODEBUG_CMD = "--nodebug")
   env.Replace(ARMBIN_NOLINKVIEW_CMD = "--nolinkview")
   env.Replace(ARMHEX_OUTPUT_CMD = "-o")
   #env.Replace(ARMHEX_OUTPUT_CMD = " ")
   env.Replace(ARMHEX_I32_CMD = "--i32")
   #env.Replace(ARMHEX_I32_CMD = "--output-target=ihex")


   #----------------------------------------------------------------------------
   # Compiler/assembler debug options
   #----------------------------------------------------------------------------
   # see above

   #----------------------------------------------------------------------------
   # ARM Procedure Call Standard (APCS) options
   #----------------------------------------------------------------------------

   env.Replace(ARM_PCSZ = "32")                       # Program counter (PC) size (bits)

   #if "USES_STACKCHK" in env:
   #   env.Replace(ARM_STACKCHK = "swst")              # software stack checking support
   #else:
   #   env.Replace(ARM_STACKCHK = "noswst")            # No software stack checking support

   #env.Replace(ARM_FRAMEPTR = "-fomit-frame-pointer")                 # No frame pointer
   env.Replace(ARM_FRAMEPTR="nofp")                 # No frame pointer
   env.Replace(ARM_FLOATPNT = "softfp")               # Use software floating point library
   #env.Replace(ARM_INTRWORK = "-mthumb-interwork")            # ARM/Thumb interworking support
   env.Replace(ARM_INTRWORK="interwork") # ARM/Thumb interworking support
   #env.Replace(ARM_NARROW   = "narrow")               # Use caller parameter narrowing
   #env.Replace(ARM_REENTRNT = "nonreentrant")         # No support for re-entrancy
   #env.Replace(ARM_NOROPI = "noropi")
   #env.Replace(ARM_NORWPI = "norwpi")

   #if env['MSM_ID'] in ['6695']:
      # --apcs /interwork/noropi/norwpi/noswst
      #env.Replace(ARMCC_APCS = "--apcs /${ARM_INTRWORK}/${ARM_NOROPI}/${ARM_NORWPI}/${ARM_STACKCHK}")
   #
   #else:
   #   env.Replace(ARMCC_APCS = "--apcs /${ARM_STACKCHK}/${ARM_INTRWORK}")
   #
   #env.Replace(ARMCXX_APCS = "--apcs /${ARM_INTRWORK}/${ARM_NOROPI}/${ARM_NORWPI}/${ARM_STACKCHK}")

   #----------------------------------------------------------------------------
   # Compiler optimization options
   #----------------------------------------------------------------------------
   env.Replace(ARM_OPT_TIME = "-Otime")
   env.Replace(ARM_OPT_SPACE = "-Osize")
   #env.Replace(ARM_OPT_3 = "-Os")
   #env.Replace(ARM_OPT_2 = "-Os")
   #env.Replace(ARM_OPT_1 = "-Os")
   #env.Replace(ARM_OPT_SPACE="-Ospace")
   env.Replace(ARM_OPT_3="-O3")
   env.Replace(ARM_OPT_2="-O2")
   env.Replace(ARM_OPT_1="-O1")
   # ARM_OPT_STACK_GUARD must be left uninitalized or it will be clobbered
   env.Replace(ARMCC_OPT = "${ARM_OPT_TIME} ${ARM_OPT_SPACE} ${ARM_OPT_STACK_GUARD}")

   # the other opts found
   env.Replace(ARMCXX_OPT = "${ARM_OPT_SPACE} ${ARM_OPT_3} ${ARM_OPT_2} ${ARM_OPT_STACK_GUARD}")

   env.Replace(ARMASM_OPT = "${ARM_OPT_1}")

   #----------------------------------------------------------------------------
   # Compiler code generation options
   #----------------------------------------------------------------------------
   #env.Replace(ARM_ALIGN = "--memaccess -UL41")
   #env.Replace(ARM_NOINLINE = "--fno-inline")
   env.Replace(ARM_NOINLINE="--no_inline")
   #env.Replace(ARM_NOAUTOINLINE = "--no_autoinline")
   #env.Replace(ARM_NO_UNALIGNED_ACCESS = "--no_unaligned_access")
   #env.Replace(ARM_NOPT = "--force_new_nothrow")      # this is added for CR 139116
   #env.Replace(ARM_END = "--littleend")               # Compile for little endian memory architecture
   #env.Replace(ARM_ZA  = "-za1")                      # LDR may only access 32-bit aligned addresses
   #env.Replace(ARM_ZAS = "-zas4")                     # Min byte alignment for structures
   #env.Replace(ARM_ZAP = "-zap0")                     # Struct ptrs NOT assumed to be aligned per -zas
   #env.Replace(ARM_ZAT = "-zat1")                     # Min byte alignment for top-level static objects
   #env.Replace(ARM_ZZT = "-zzt0")                     # Force uninitialized variables to ZI data area
   #env.Replace(ARM_SPLIT_SECTIONS = "-ffunction-sections")
   #env.Replace(ARM_SPLIT_SECTIONS="--split_sections")
   #env.Replace(ARM_FPMODE_FAST = "--fpmode fast")
   #env.Replace(ARM_DATA_REORDER = "--data_reorder")
   #env.Replace(ARM_LOOSE_IMPLICIT_CAST = "--loose_implicit_cast")



   if ('BUILD_BOOT_CHAIN' in env) or ('BUILD_TOOL_CHAIN' in env):
      env.Replace(ARMCC_CODE="${ARM_END} ${ARM_ALIGN}")
   elif env['MSM_ID'] in ['6695']:
      # --fpmode fast --littleend --no_inline --no_autoinline --split_sections --data_reorder
      env.Replace(ARMCC_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_NOINLINE} " \
         "${ARM_NOAUTOINLINE} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER}")
   else:
      env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_NOPT}")

   env.Replace(ARMCXX_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_NOINLINE} " \
      "${ARM_NOAUTOINLINE} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER} ${ARM_LOOSE_IMPLICIT_CAST}")

   #---------------------------------------------------------------------------
   #  Stack Protection
   #---------------------------------------------------------------------------
   if 'USES_STACK_PROTECTOR' in env:
      env.Replace(RVCT_STKPRO="--protect_stack")
   else:
      env.Replace(RVCT_STKPRO="")

   """
   # already handled by build_dflags?
   #---------------------------------------------------------------------------
   # common defines
   #---------------------------------------------------------------------------
   if 'CUST_H' in env:
      env.Replace(CUST_H_CPPDEFINE='-DCUST_H=\\"${CUST_H}\\"')

   if 'BUILD_ID' in env:
      env.Replace(BUILD_TARGET_CPPDEFINE='-DBUILD_TARGET=\\"${BUILD_ID}\\"')

   if 'BUILD_VER' in env:
      env.Replace(BUILD_VER_CPPDEFINE='-DBUILD_VER=\\"${BUILD_VER}\\"')

   if 'BUILD_ASIC' in env:
      env.Replace(DBUILD_ASIC_CPPDEFINE='-DBUILD_ASIC=\\"${BUILD_ASIC}\\"')
   """
   env.Replace(BUILD_CCFLAGS='${CUST_H_CPPDEFINE} ' \
      '${BUILD_TARGET_CPPDEFINE} ${BUILD_VER_CPPDEFINE} ' \
      '${DBUILD_ASIC_CPPDEFINE} -DBUILD_LICENSE_ENVIRONMENT=' \
      'NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 -DGPL_ENVIRONMENT=2 ' \
      '-D__FILENAME__=\\"$SOURCE.file\\" ')
   """
   env.Replace(BUILD_ASMFLAGS='${CUST_H_CPPDEFINE} ' \
      '${BUILD_TARGET_CPPDEFINE} ${BUILD_VER_CPPDEFINE} ' \
      '${DBUILD_ASIC_CPPDEFINE} -D__FILENAME__=\\"$SOURCE.file\\" ')
   """

   #----------------------------------------------------------------------------
   # Linker options
   #----------------------------------------------------------------------------

   #----------------------------------------------------------------------------
   # NOTE:
   # The -zap0 option is required due to a known ARM 2.50 compiler bug with
   # pointers to struct sub-fields within __packed structs
   #----------------------------------------------------------------------------

   env.Replace(BUILD_FLAGS = "-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 -DGPL_ENVIRONMENT=2")

   env.Replace(INCPREFIX = "-I")
   env.Replace(CPPDEFPREFIX  = "-D")
   env.Replace(OBJSUFFIX = ".o")

   env.Replace(LIBPREFIX = "")
   env.Replace(LIBSUFFIX = ".lib")
   env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a'])
   env.Replace(LIBLIKNPREFIX = "")
   env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
   env.Replace(LIBDIRPREFIX = "-L ")

   env.Replace(PROGPREFIX = "")
   env.Replace(PROGSUFFIX = ".elf")

   # max cmd line lenght
   # on a cmd sh for XP or newer max comand line is 8191, on 2000 and NT is 2047
   # usig CreateProcess (our internal spawn) 32766.
   # POSIX OSs is no issue command line limit usually in the 128k range.
   # cygwin has the added restriction of max 32k including OS env variables
   # we have seen a lot of issues with OS environment grater then 20k
   # (What is going on from MAKE world why is environment so large??)
   if env['PLATFORM'] in ['windows', 'win32']:
      env.Replace(MAXLINELENGTH = 32000)
   elif env['PLATFORM'] in ['cygwin']:
      dyn_maxlinelength = 32000 - len(str(os.environ))
      if dyn_maxlinelength < 2000:
         dyn_maxlinelength = 2000
      env.Replace(MAXLINELENGTH = dyn_maxlinelength)
   else:
      env.Replace(MAXLINELENGTH = 100000)

   # WARNING!! cmdn can't have any shell operation such as "<", ">" or "|"
   # if it does we will default back os shell and its cmd lenght restrictions
   # will be in effect.

   # Ecapsulate within quotes so we retain the space after --via
   #env.Replace(TEMPFILEPREFIX = "\"--via \"")
   env.Replace(TEMPFILEPREFIX = "@")
   env.Replace(TEMPFILESUFFIX = "")

   # Add a way to expand link files from a list...
   #env.Replace(LISTFILEPREFIX = "\"--via \"")
   env.Replace(LISTFILEPREFIX = "@")
   env.Replace(LISTFILESUFFIX = "")
   env.Replace(_LISTFILES = '${_concat(LISTFILEPREFIX, LISTFILES, LISTFILESUFFIX, __env__)}')

   # Init FLAGS
   env.Append(CFLAGS = "-fuse-baremetal-sysroot -fdata-sections -ffunction-sections -fomit-frame-pointer -fshort-enums -mllvm -enable-gvn-hoist")
   env.Replace(CCFLAGS = "$ARMCC_ARCH $ARMCC_APCS $ARMCC_CODE $ARM_DBG $ARMCC_OPT $BUILD_CCFLAGS $BUILD_FLAGS $CC_DFLAGS $CFLAGS -std=c99")
   #env.Replace(SHCCFLAGS= "$ARMCC_CPU $ARMCC_APCS $ARMCC_CODE $ARM_DBG $ARMCC_OPT $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $RVCT_STKPRO $ARMLD_SH_FPIC ")
   env.Replace(CXXFLAGS = "$ARMCC_ARCH $ARMCXX_APCS $ARMCXX_CODE $ARM_DBG $ARMCXX_OPT $BUILD_FLAGS $CC_DFLAGS $CFLAGS -fno-exceptions -fno-unwind ")
   env.Replace(ASCPPFLAGS = "$ARMCC_ARCH $ARMCC_APCS $ARMASM_OPT $BUILDFLAGS $ASM_DFLAGS")
   #env.Replace(SHCXXFLAGS= "$ARMCC_CPU $ARMCXX_APCS $ARMCXX_CODE $ARM_DBG $ARMCXX_OPT $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $RVCT_STKPRO $ARMLD_SH_FPIC ")
   env.Replace(ASMFLAGS = "$ARMASM_CPU $ARMCC_APCS $ARMASM_OPT $BUILDFLAGS $ASM_DFLAGS")
   env.Replace(ASFLAGS = "--cpu $ARM_CPU $ARMCC_APCS $ASM_DBG")
   if UseClangAs:
      env.Replace(ASFLAGS = " ")
   env.Replace(ARFLAGS = "$ARMAR_LIB_REPLACE $ARMAR_LIB_QUITE")
   env.Replace(LINKFLAGS = "-fuse-baremetal-sysroot -fno-use-baremetal-crt -static  -Wl,-no-undefined -Wl,-T,${TARGET.base}.scl -Wl,-Map,${TARGET.base}.map -Wl,-gc-sections  ${LIBDIRPREFIX} ${LLVMLIB}/clang/3.9.3/lib/linux")
  
   if do_64:
     cccom = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVMINCLUDE64 $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   else:
     cccom = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVMINCLUDE $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"

   env.Replace(CCCOM =       cccom)
   env.Replace(CCCOMCPY =    cccom)
   env.Replace(CCPPCOM =     cccom.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CCMIXCOM =    cccom.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")

   # TODO: need to remove CCFALGS from CXXCOM it is couseing a lot of warnings, a lot of files in MULTIMEDIA need to be fixed first
   #CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   cxxcom = "${TEMPFILE('$CXX $CXXFLAGS $CCFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(CXXCOM =       cxxcom)
   env.Replace(CXXCOMCPY =    cxxcom)
   env.Replace(CXXPPCOM =     cxxcom.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CXXMIXCOM =    cxxcom.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])

   env.Replace(ARCOM =      "${TEMPFILE('$AR $ARFLAGS $TARGET $SOURCES')}")
   env.Replace(LINKCOM =    "${TEMPFILE('$LINK $LINKFLAGS $_LIBDIRFLAGS $SOURCES -Wl,--start-group $_LISTFILES -Wl,--end-group  -Wl,--start-group $_LIBFLAGS  -Wl,--end-group $ARMLD_OUTPUT_CMD $TARGET')}")

   """
   env.Replace(SHLIBEMITTER="")
   shcccom = "${TEMPFILE('$SHCC $SHCCFLAGS $_CCCOMCOM $INCPATHS " \
      "$ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(SHCCCOM=shcccom)
   env.Replace(SHCCCOMCPY=shcccom)
   env.Replace(SHCCPPCOM=shcccom.replace('$HEXAGON_COMPILE_CMD', \
      '$HEXAGON_PREPROCESS_CC_CMD $PPFLAGS'))
   env.Replace(SHCCMIXCOM=shcccom.replace('$HEXAGON_COMPILE_CMD', \
      '$HEXAGON_ASMOUT_CMD'))
   env.Replace(SHCCPPCLCOM="$PPASM -f $SOURCES.posix -o $TARGET.posix")
   env.Replace(SHCCPPCL_ACOM= \
      "$PPASM -f ${TARGET.posix}.pp -o ${TARGET.posix}.i")
   env.Replace(SHHDEPTREECOM="$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")
   shcxxcom = "${TEMPFILE('$SHCXX $SHCXXFLAGS $_CCCOMCOM $INCPATHS "\
      "$ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(SHCXXCOM=shcxxcom)
   env.Replace(SHCXXCOMCPY=shcxxcom)
   env.Replace(SHCXXPPCOM=shcxxcom.replace('$HEXAGON_COMPILE_CMD', \
      '$HEXAGON_PREPROCESS_CXX_CMD $PPFLAGS'))
   env.Replace(SHCXXMIXCOM=shcxxcom.replace('$HEXAGON_COMPILE_CMD', \
      '$HEXAGON_ASMOUT_CMD'))
   env.Replace(SHCXXPPCLCOM=env['CCPPCLCOM'])
   env.Replace(SHCXXPPCL_ACOM=env['CCPPCL_ACOM'])
   env.Replace(ARCOM="${TEMPFILE('$AR $ARFLAGS $TARGET $SOURCES')}")
   env.Replace(LINKCOM="${TEMPFILE('$LINK $LINKFLAGS $_LIBDIRFLAGS " \
      "$_LISTFILES $_LIBFLAGS  $SOURCES $ARMLD_OUTPUT_CMD $TARGET')}")
   env.Replace(SHLINKCOM="${TEMPFILE('$SHLINK $SHLINKFLAGS $SOURCES " \
      "$ARMLD_OUTPUT_CMD $TARGET')}")
   """

   env.Replace(ASCPPCOM="$TCC $ASMFLAGS $AFLAGS $_CCCOMCOM $INCPATHS " \
      "$ARMCC_PREPROCESS_CMD $SOURCES $ARMCC_OUTPUT_CMD ${TARGET}.pp")
   env.Replace(ASASCOM="$AS $ASFLAGS $_CPPINCFLAGS $INCPATHS " \
      "${TARGET}.i $ARMAS_OUTPUT_CMD $TARGET")

   env.Replace(ASPPCOMCPY=['$ASCPPCOM', '$ASASCOM'])

   asm_preprocess_action = \
      SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
   asm_clean_action = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
   asm_assembly_action = \
      SCons.Action.Action('${TEMPFILE("$ASASCOM")}', '$ASASCOMSTR')

   env.Replace(ASCOM=[
      asm_preprocess_action,        # C pre-process assembly files
      asm_clean_action,             # Removed junk generated by C preprocess
      asm_assembly_action,          # Do the actual assembly
      # Do not delete here needed to debug on target
      #env.GetDeleteAction('${TARGET}.i'),
      #env.GetDeleteAction('${TARGET}.pp'),
      #env.GetDeleteAction('${TARGET}.lst'),
   ])

   ascom = "$AS $ASFLAGS $_CPPINCFLAGS $INCPATHS " \
      " $SOURCES $ARMAS_OUTPUT_CMD $TARGET"
   #env.Replace(ASCOM_TEMP=ascom)
   #env.Replace(ASCOM="${TEMPFILE('$ASCOM_TEMP')}")
   env.Replace(ASCOMCPY=ascom)

   # avoid having a rallib run for no reason, scons defaults does this, but is maybe casing issues
   # as there is no actual ranlibcom
   env.Replace(RANLIBCOM = "")
   env.Replace(RANLIBCOMSTR = "")

   #============================================================================
   # COMMON CC ADDITIONS
   #============================================================================
   comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

   if env.PathExists(comcc_addt):
      env.LoadToolScript(comcc_addt)

   #============================================================================
   # Other options
   #============================================================================
   callgraph = env.GetOption('callgraph')

   if not (LinkByGNU or LinkByQCLD):
      env.Append(LINKFLAGS = " --force-scanlib")
   if callgraph:
      env.Append(LINKFLAGS = " $ARMLD_CALLGRAPH_CMD")

   #============================================================================
   # ALTERNATE PRODUCT OPTIONS
   #============================================================================
   llvm_product = None

   if env.PathExists("${BUILD_SCRIPTS_ROOT}/llvm_product.py"):
      llvm_product = "${BUILD_SCRIPTS_ROOT}/llvm_product.py"
   elif env.PathExists("${BUILD_MS_ROOT}/llvm_product.py"):
      llvm_product = "${BUILD_MS_ROOT}/llvm_product.py"

   if llvm_product is not None:
      env.LoadToolScript(llvm_product)

   #============================================================================
   # ALTERNATE OEM OPTIONS
   #============================================================================
   llvm_oem = "${BUILD_SCRIPTS_OEM_ROOT}/llvm_oem.py"

   if env.PathExists(llvm_oem):
      env.LoadToolScript(llvm_oem)


   #print env.Dump()


   '''Possible TARGET.attribute or SOURCE.attributes

   dictSpecialAttrs = { "base"     : __get_base_path,
                        "posix"    : __get_posix_path,
                        "windows"  : __get_windows_path,
                        "win32"    : __get_windows_path,
                        "srcpath"  : __get_srcnode,
                        "srcdir"   : __get_srcdir,
                        "dir"      : __get_dir,
                        "abspath"  : __get_abspath,
                        "filebase" : __get_filebase,
                        "suffix"   : __get_suffix,
                        "file"     : __get_file,
                        "rsrcpath" : __get_rsrcnode,
                        "rsrcdir"  : __get_rsrcdir,
                      }
   '''
