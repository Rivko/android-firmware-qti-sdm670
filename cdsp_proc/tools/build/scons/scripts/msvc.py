'''
   SCons MSVC tool rules
'''
#===============================================================================
#
# SCons MSVC tool rules
#
# GENERAL DESCRIPTION
#    HEXAGON Tools definitions
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/msvc.py#3 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 02/02/11   enj     Initial version
#
#===============================================================================
import os, string, tempfile
import SCons.Action
from SCons.Script import *

#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
    '''
       C_static_obj_emitter

       Chain emitter for C files
       Adds explicit dep
    '''
    #import pdb; pdb.set_trace()

    # call defualt static object emitter
    tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)

    preprocess = env.GetOption('preprocess')
    mixasm = env.GetOption('mixasm')
    hdeptree = env.GetOption('hdeptree')

    if preprocess:
        env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
        env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))

    if mixasm:
        env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))

    if hdeptree:
        env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))

    if 'EXPLICIT_DEPS' in env:
        env.Depends(tgt, env['EXPLICIT_DEPS'])

    return (tgt, src)

#------------------------------------------------------------------------------
# Chain emitter for asm files
# Adds explicit dep
#------------------------------------------------------------------------------
def S_static_obj_emitter(target, source, env):
    '''
       S_static_obj_emitter

       Chain emitter for asm files
       Adds explicit dep
    '''
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

#-------------------------------------------------------------------------------
# Add Program (Link into exe, elf, etc).
#-------------------------------------------------------------------------------
def add_program(env, target=None, source=None, **kargs):
    '''
       add_program
    '''

    #---------------------------------------------------------------------------
    # Init vars and eviroment needed to use Program builder
    image_env = env.get('IMAGE_ENV')

    # variables for Program builder
    if image_env is not None:
        if target is None:
            target = image_env.get('TARGET_NAME')

        if source is None:
            objs = image_env.get('IMAGE_OBJS')
        else:
            objs = source

        if 'LIBS' not in kargs:
            libs = image_env.get('IMAGE_LIBS')
            #kargs['LIBS'] = image_env.get('IMAGE_LIBS')
        else:
            libs = kargs['LIBS']

        del kargs['LIBS']

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

    if 'USES_ANALYTIC_DATA' in env:
        srcs_lf_fname = env.subst("%s_src.lf" % target_base)
        sources_list_file = env.SourceListBuilder(srcs_lf_fname, [objs, libs], \
            add_header=True, relative_path=build_dir, posix=True)
        env.Depends(target_elf, sources_list_file)

    # add depndency on external buildsystem files
    exbldsys_files = None
    if len(list_files) > 0:
        exbldsys_list_fname = env.subst("%s_exbldsys.lf" % target_base)
        exbldsys_files = env.ListFileExtractor(exbldsys_list_fname, list_files)
        env.Depends(target_elf, exbldsys_files)
        env.Append(LISTFILES=[exbldsys_files[0].path])

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

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('msvc')

def generate(env):
    '''
       generate
    '''
    #===========================================================================
    # figure out hwta tools set will be using
    #===========================================================================
    # The following environment variables must be defined prior to using this
    # make file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
    # the GCC tools.

    # Load the MSVC 8.0 (VS 2005) toolset
    env["TOOL"] = 'vc8'
    env["MSVS"] = {"VERSION": "8.0"}
    env["MSVS_VERSION"] = "8.0"
    for tool in ['msvc', 'mslib', 'mslink', 'nasm']:
        SCons.Script.Tool(tool)(env)

    # since we may have change to cygwin re init formatting
    env.LoadToolScript('formatting', \
                        toolpath=['${BUILD_ROOT}/tools/build/scons/scripts'])

    #===========================================================================
    # Special setup for Windows MSVC
    #===========================================================================
    if 'INCLUDE' in os.environ:
        env['ENV']['INCLUDE'] += os.pathsep + os.environ['INCLUDE']
    else:
        env.PrintError('MSVS not initialized properly.\nEnvironment variable ' \
                       '\'INCLUDE\' not set.\n\n')
        raise RuntimeError("nEnvironment variable 'INCLUDE' not set.")
    if 'LIB' in os.environ:
        env['ENV']['LIB'] += os.pathsep + os.environ['LIB']
    else:
        env.PrintError('MSVS not initialized properly.\nEnvironment variable ' \
                       '\'LIB\' not set.\n\n')
        raise RuntimeError("nEnvironment variable 'LIB' not set.")

    # init variables to environment
    ## env.Replace(HEXAGON_ROOT = hexagon_root)
    ## env.Replace(HEXAGON_IMAGE_ENTRY = hexagon_image_entry)
    ## env.Replace(HEXAGON_RTOS_RELEASE = hexagon_rtos_release)
    ## env.Replace(HEXAGON_TOOLS = hexagon_tools)
    ## env.Replace(QDSP6_RELEASE_DIR = \
    ##     env.RealPath("${HEXAGON_ROOT}/${HEXAGON_RTOS_RELEASE}", posix=True))
    ## env.Replace(Q6VERSION = q6version)
    ## env.Replace(L4Q6_ROOT = \
    ##     env.RealPath("${L4_ROOT}/../l4-qdsp6", posix=True))

    ## if not env.PathExists("${QDSP6_RELEASE_DIR}"):
    ##    env.PrintError("---------------------------------------------------" \
    ##        "----------------------------")
    ##    env.PrintError("*** HEXAGON tools are not present")
    ##    env.PrintError \
    ##        ("*** Please install HEXAGON tools RTOS 1.1, 2.0Beta, etc.")
    ##    env.PrintError \
    ##        ("*** Currently setup for RTOS: ${HEXAGON_RTOS_RELEASE}")
    ##    env.PrintError("*** Missing RTOS PATH: ${QDSP6_RELEASE_DIR}")
    ##    env.PrintError("---------------------------------------------------" \
    ##        "----------------------------")
    ##    raise RuntimeError, \
    ##        "HEXAGON tools are not installed on this environment"

    #===========================================================================
    # Special init for our tools and special dependency in CUST_H
    #===========================================================================
    # Replace Static Object Emitter
    env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
    env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
    env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)

    # Standard include paths always searched
    env.Append(CPPPATH=[
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
            env.Append(EXPLICIT_DEPS=[cust_h_file])
            deps = cust_scanner(cust_h_file, env, cust_path)
            if deps:
                env.Append(EXPLICIT_DEPS=deps)

    if 'AMSS_CUST_H' in env:
        cust_h_file = env.FindFile(env['AMSS_CUST_H'], env['CPPPATH'])
        if cust_h_file:
            env.Append(EXPLICIT_DEPS=[cust_h_file])
            deps = cust_scanner(cust_h_file, env, cust_path)
            if deps:
                env.Append(EXPLICIT_DEPS=deps)

    #env.PrintInfo("HEXAGON_TOOLS       = ${HEXAGON_TOOLS}")
    #env.PrintInfo("HEXAGON_RTOS_REL    = ${HEXAGON_RTOS_RELEASE}")
    #env.PrintInfo("Q6VERSION           = ${Q6VERSION}")
    #env.PrintInfo("HEXAGON_ROOT        = %s" % env.RealPath("${HEXAGON_ROOT}"))
    #env.PrintInfo("QDSP6BIN            = %s" % env.RealPath("${QDSP6BIN}"))
    #env.PrintInfo("HEXAGON_IMAGE_ENTRY = ${HEXAGON_IMAGE_ENTRY}")

    # Preprocess assembly cleaner
    env.Replace(PPASM_FILE=env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))
    env.Replace(PPASM="${PYTHONCMD} ${PPASM_FILE}")

    #---------------------------------------------------------------------------
    # Compiler/assembler debug options
    #---------------------------------------------------------------------------
    if env.has_key('USES_NO_DEBUG'):
        env.Replace(MSVC_DBG="")
    else:
        env.Replace(MSVC_DBG="/Zi")

    #---------------------------------------------------------------------------
    # Warnings Options
    #---------------------------------------------------------------------------
    env.Replace(MSVC_WARN="/W3 /we4002 /we4020")

    #---------------------------------------------------------------------------
    # Optimization Options
    #---------------------------------------------------------------------------
    env.Replace(MSVC_OPT="/Od")

    #---------------------------------------------------------------------------
    # Misc Options
    #---------------------------------------------------------------------------
    env.Replace(MSVC_MISC="/nologo /EHa /RTCsu /MTd")

    #---------------------------------------------------------------------------
    # Architecture defines
    #---------------------------------------------------------------------------
    env.Replace(MSVC_DEFS="/DT_WINNT /DWIN32 /D_CONSOLE /D_DEBUG " \
                          "/DWINVER=0x0501 /D_WIN32_WINNT=0x0501 /D__i386__")

    #---------------------------------------------------------------------------
    # HEXAGON Flags Def.
    #---------------------------------------------------------------------------
    # max cmd line length
    # on a cmd sh for XP or newer max command line is 8191, on 2000 and NT is
    # 2047 using CreateProcess (our internal spawn) 32766.
    env.Replace(MAXLINELENGTH=8000)

    # For file redirection, use custom MSVC temp file munger.
    env["TEMPFILE"] = MSVCTempFileMunge

    # WARNING!! cmdn can't have any shell operation such as "<", ">" or "|"
    # if it does we will default back os shell and its cmd lenght restrictions
    # will be in effect (8191).

    env.Replace(TEMPFILEPREFIX="@")
    env.Replace(TEMPFILESUFFIX="")

    # Init FLAGS
    env.Replace(CCFLAGS="$MSVC_MISC $MSVC_WARN $MSVC_DBG $MSVC_OPT " \
                        "$MSVC_DEFS $BUILD_FLAGS $CC_DFLAGS $CFLAGS ")
    env.Replace(CXXFLAGS="$MSVC_MISC $MSVC_WARN $MSVC_DBG $MSVC_OPT " \
                         "$MSVC_DEFS $BUILD_FLAGS $CC_DFLAGS $CFLAGS ")
    env.Replace(ASMFLAGS="$MSVC_MISC $MSVC_WARN $MSVC_DBG $MSVC_OPT " \
                         "$MSVC_DEFS $BUILD_FLAGS $CC_DFLAGS ")
    env.Replace(ASFLAGS="$MSVC_MISC $MSVC_WARN $MSVC_DBG $MSVC_OPT " \
                        "$MSVC_DEFS $BUILD_FLAGS $CC_DFLAGS ")
    #env.Replace(ARFLAGS="$HEXAGONAR_LIB_QUITE $HEXAGONAR_LIB_REPLACE")

    env.Replace(CCCOMCPY='$CCCOM')
    env.Replace(CXXCOMCPY='$CXXCOM')
    env.Replace(CCPPCOM="${TEMPFILE('$CC /E /c $SOURCES $CFLAGS " \
                        "$CCFLAGS $_CCCOMCOM > $TARGET')}")
    env.Replace(CXXPPCOM="${TEMPFILE('$CXX /E /c $SOURCES $CXXFLAGS " \
                         "$CCFLAGS $_CCCOMCOM > $TARGET')}")
    env.Replace(CCPPCLCOM="$PPASM -f $SOURCES.posix -o $TARGET.posix")
    env.Replace(CCPPCL_ACOM= \
        "$PPASM -f ${TARGET.posix}.pp -o ${TARGET.posix}.i")
    env.Replace(HDEPTREECOM="$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")

    env.Replace(CXXPPCLCOM=env['CCPPCLCOM'])
    env.Replace(CXXPPCL_ACOM=env['CCPPCL_ACOM'])

    asm_preprocess_action = \
        SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
    asm_clean_action = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
    asm_assembly_action = SCons.Action.Action('$ASASCOM', '$ASCOMSTR')

    env.Replace(ASCOM=[ \
        #   asm_preprocess_action,     # C pre-process assembly files
        #   asm_clean_action,          # Removed junck generated by C preprocess
        asm_assembly_action,           # Do the actuall assymbly
        #   env.GetDeleteAction('${TARGET.posix}.pp'),
        #   env.GetDeleteAction('${TARGET.posix}.lst'),
    ])

    #---------------------------------------------------------------------------
    # MOB LINK, very special procedure
    #---------------------------------------------------------------------------
    # LINK special case in hexagon world
    env.Append(LINKFLAGS='/NOLOGO')
    env.Append(LINKFLAGS='/SUBSYSTEM:CONSOLE')
    env.Append(LINKFLAGS='/MACHINE:X86')
    env.Append(LINKFLAGS='/DEBUG')
    env.Append(LINKFLAGS='/OPT:NOREF')

    env.Append(LINKFLAGS='/NODEFAULTLIB:msvcrt')
    env.Append(LINKFLAGS='/NODEFAULTLIB:msvcprt')
    env.Append(LINKFLAGS='/NODEFAULTLIB:msvcrtd')
    env.Append(LINKFLAGS='/NODEFAULTLIB:msvcprtd')
    env.Append(LINKFLAGS='/NODEFAULTLIB:libcmt')
    env.Append(LINKFLAGS='/DEFAULTLIB:libcmtd')
    env.Append(LINKFLAGS='/DEFAULTLIB:libcpmtd')

    env.Append(LIBS=['Winmm', 'Ws2_32', 'user32.lib', 'Advapi32.lib'])

    #===========================================================================
    # COMMON CC ADDITIONS
    #===========================================================================
    comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

    if env.PathExists(comcc_addt):
        env.LoadToolScript(comcc_addt)

class MSVCTempFileMunge(object):
    '''A callable class.  You can set an Environment variable to this,
       then call it with a string argument, then it will perform temporary
       file substitution on it.  This is used to circumvent the long command
       line limitation.

       Example usage:
       env["TEMPFILE"] = MSVCTempFileMunge
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

           Constructor
        '''
        self.cmd = cmd
        self.verbose = int(GetOption('verbose'))

    def __call__(self, target, source, env, for_signature):
        '''
           __call__
        '''
        # If signature is desired, return immediately
        if for_signature:
            return self.cmd
        # Use the command passed and substitute variables to get a list
        cmd = env.subst_list(self.cmd, 0, target, source)[0]

        # What is our max line length
        try:
            maxline = int(env.subst('$MAXLINELENGTH'))
        except ValueError:
            maxline = 2048

        # If we're not greater than the max line length, do nothing
        if (reduce(lambda x, y: x + len(y), cmd, 0) + len(cmd)) <= maxline:
            return self.cmd

        # We do a normpath because mktemp() has what appears to be a
        # bug in Windows that will use a forward slash as a path
        # delimiter.  Windows's link mistakes that for a command line
        # switch and barfs.
        #
        # We use the .scons.tmp suffix for the benefit of the Phar Lap
        # linkloc linker, which likes to append an .lnk suffix if none
        # is given.
        tmp = os.path.normpath(tempfile.mktemp('.scons.tmp'))
        native_tmp = SCons.Util.get_native_path(tmp)

        # See if there's a redirection('>') in the command
        # since it can't go in the tempfile
        capture_stdout = False
        for arg in cmd:
            if capture_stdout == True:
                capture_stdout = arg
            if arg == '>':
                capture_stdout = True
        # If redirecting, remove from the args list
        if capture_stdout:
            cmd.remove('>')
            cmd.remove(capture_stdout)

        # Don't use 'rm' if the shell is not sh, because rm won't
        # work with the Windows shells (cmd.exe or command.com) or
        # Windows path names.
        remove = 'del'

        prefix = env.subst('$TEMPFILEPREFIX')

        args = map(SCons.Subst.quote_spaces, cmd[1:])
        open(tmp, 'w').write(string.join(args, " ") + "\n")

        # Return an action based on a list
        action = [cmd[0]]
        verbose_cmd_str = "[\'" + str(cmd[0]) + " "

        # Add the tempfile redirection
        action.append(prefix + native_tmp)
        verbose_cmd_str += prefix + native_tmp

        # Add stdout redirection if necessary
        if capture_stdout:
            action.append('>')
            action.append(capture_stdout)
            verbose_cmd_str += " > " + capture_stdout

        # Newline and delete tempfile
        action.append('\n')
        action.append(remove)
        action.append(native_tmp)
        verbose_cmd_str += "\'],[\'" + remove + " " + native_tmp + "\']"

        if SCons.Action.print_actions and self.verbose > 1:
            print "** Using tempfile "+ native_tmp
            print "** CMD: " + verbose_cmd_str

        #import pdb; pdb.set_trace()

        # Return the action
        return action
