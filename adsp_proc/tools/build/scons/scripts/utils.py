'''
   Common utils

   GENERAL DESCRIPTION
      Common utilities used across the build system
'''
#==============================================================================
#
# Common utils
#
# GENERAL DESCRIPTION
#    Common utilities used across the build system
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/utils.py#26 $
#  $DateTime: 2018/03/19 11:31:19 $
#  $Author: pwbldsvc $
#  $Change: 15713454 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/15/09   wd      Create
#
#==============================================================================
import os
import glob
import re
import hashlib
import subprocess
import string
import fnmatch
import shutil
import textwrap
import imp
import errno
import SCons.Action
#import SCons.Errors
#import SCons.Tool
from SCons.Script import *
from datetime import datetime
import stat
import inspect
import json
import time
import threading

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
    exists
    '''
    return env.Detect('utils')

def generate(env):
    '''
    generate
    '''
    # init utils methods
    env.AddMethod(check_alias, "CheckAlias")
    env.AddMethod(bind_aliases_to_targets, "BindAliasesToTargets")
    env.AddMethod(get_delete_action, "GetDeleteAction")

    # add libs, objs, files, artifacts methods
    env.AddMethod(is_key_enable, "IsKeyEnable")
    env.AddMethod(is_key_enable, "IsTargetEnable")
    env.AddMethod(is_key_enable, "IsTagEnable")
    env.AddMethod(is_build_internal, "IsBuildInternal")
    env.AddMethod(add_file, "AddFile")
    env.AddMethod(add_library, "AddLibrary")
    env.AddMethod(add_bin_library, "AddBinaryLibrary")
    env.AddMethod(AnalyzeLibrary, "AnalyzeLibrary")
    env.AddMethod(add_oem_library, "AddOEMLibrary")
    env.AddMethod(add_object, "AddObject")
    env.AddMethod(add_bin_object, "AddBinaryObject")
    env.AddMethod(add_oem_object, "AddOEMObject")
    env.AddMethod(add_document, "AddDocument")
    env.AddMethod(add_listfile, "AddListFile")
    env.AddMethod(add_artifact, "AddArtifact")
    env.AddMethod(add_program, "AddProgram")
    env.AddMethod(add_nodes_to_list, "AddNodestoList")

    # add libs, objs (deprecated soon to be removed)
    env.AddMethod(add_libs_to_image, "AddLibsToImage")
    env.AddMethod(add_objs_to_image, "AddObjsToImage")
    env.AddMethod(add_libs_to_image, "AddLibs")
    env.AddMethod(add_objs_to_image, "AddObjs")
    env.AddMethod(add_docs_to_image, "AddDocs")

    # init print/text utils methods
    env.AddMethod(print_stage_banner, "PrintStageBanner")
    env.AddMethod(print_image_banner, "PrintImageBanner")
    env.AddMethod(print_error, "PrintError")
    env.AddMethod(print_warning, "PrintWarning")
    env.AddMethod(print_info, "PrintInfo")
    env.AddMethod(print_debug_info, "PrintDebugInfo")
    env.AddMethod(print_data, "Print")
    env.AddMethod(create_file_banner, "CreateFileBanner")

    # init file/path utils methods
    env.AddMethod(find_files, "FindFiles")
    env.AddMethod(find_source_files, "FindSourceFiles")
    env.AddMethod(glob_files, "GlobFiles")
    env.AddMethod(glob_source_files, "GlobSourceFiles")
    env.AddMethod(add_explicit_deps, "AddExplicitDeps")
    env.AddMethod(remove_file, "RemoveFiles")
    env.AddMethod(md5file, "GetMD5SumFile")
    env.AddMethod(md5file_compare, "CompareMD5File")
    env.AddMethod(md5file_compare_and_update, "CompareAndUpdateMD5File")
    env.AddMethod(md5file_create, "CreateMD5SumFile")
    env.AddMethod(path_to_dict_tree, "PathToDictTree")
    env.AddMethod(filter_match, "FilterMatch")
    env.AddMethod(norm_path, "NormPath")
    env.AddMethod(path_exists, "PathExists")
    env.AddMethod(real_path, "RealPath")
    env.AddMethod(clean_pack, "CleanPack")

    # init file/path utils methods (deprecated soon to be removed)
    env.AddMethod(norm_path, "SubstNormPath")
    env.AddMethod(path_exists, "SubstPathExists")
    env.AddMethod(real_path, "SubstRealPath")

    # init and load software units methods
    env.AddMethod(load_scons_scripts, "LoadSConscripts")
    env.AddMethod(load_tool_script, "LoadToolScript")
    env.AddMethod(load_image_units, "LoadImageUnits")
    env.AddMethod(load_api_units, "LoadAPIUnits")
    env.AddMethod(get_au_sw_unit_names, "GetAreaSoftwareUnitNames")
    env.AddMethod(load_au_sw_units, "LoadAreaSoftwareUnits")
    env.AddMethod(load_au_sw_units, "GetAreaSoftwareUnits")
    env.AddMethod(load_sw_units, "LoadSoftwareUnits")
    env.AddMethod(get_sw_units, "GetSoftwareUnits")

    # init and load software units methods (deprecated soon to be removed)
    env.AddMethod(load_cbsp_lib_scripts_deprecated, "LoadCoreBSPLibScripts")
    env.AddMethod(load_ext_lib_scripts_deprecated, "LoadExternalLibScripts")
    env.AddMethod(load_au_sw_units_deprecated, "LoadAUSoftwareUnits")

    # init run,execute,process methods
    env.AddMethod(exec_cmds, "ExecCmds")
    env.AddMethod(run_cmds, "RunCmds")

    # Debug functions
    env.AddMethod(find_env_var_by_value, "FindEnvVarByValue")

    # Functions to store overall link time
    env.AddMethod(store_linking_start_time, "StoreLinkingStartTime")
    env.AddMethod(print_overall_link_time, "PrintOverallLinkTime")

    # service methods
    env.AddMethod(write_image_info, "WriteImageInfo")
    env.AddMethod(acquire_mp_mutex, "AcquireMPMutex")
    env.AddMethod(release_mp_mutex, "ReleaseMPMutex")
    env.AddMethod(mark_time_start, "MarkTimeStart")
    env.AddMethod(mark_time_end, "MarkTimeEnd")

    return None

#==============================================================================
# add_items_to_image
# Add libraries to image environment depending on the image name given and
# the image name(s) defined in the environment
#==============================================================================
def add_items_to_image(env, dst_list, targets, items):
    '''
    add_items_to_image

    Add libraries to image environment depending on the image name given and
    the image name(s) defined in the environment
    '''

    #import pdb; pdb.set_trace()

    if type(targets) is not list:
        targets = [targets]

    if (type(items) is not list) and (type(items) is not SCons.Node.NodeList):
        items = [items]

    intersection = set(targets) & set(env.Dictionary())
    if len(intersection) > 0:
        item_list = env.Flatten(items)
        dst_list.extend(item_list)
        for item in item_list:
            env.PrintDebugInfo('bindist',  \
                "Adding file using deprecated api: " + os.path.join( \
                os.path.relpath(os.curdir, env.subst('${BUILD_ROOT}')), \
                str(item)))

    return None

def add_nodes_to_list(env, dst_list, nodes):
    '''
    add_nodes_to_list
    '''
    #import pdb; pdb.set_trace()

    if (type(nodes) is not list) and (type(nodes) is not SCons.Node.NodeList):
        nodes = [nodes]

    node_list = env.Flatten(nodes)
    dst_list.extend(node_list)

    return None

def add_libs_to_image(env, targets, libs):
    '''
    add_libs_to_image
    '''

    #import pdb; pdb.set_trace()
    root_env = env.get('ROOT_ENV')

    # get list container for current AU or global
    # if an AU container dosen't exist
    lib_list = root_env.get('AU_LIBS')
    if lib_list is None:
        lib_list = root_env.get('LIBS')

    if 'USES_BINARY_DIST' in env:
        if type(libs) is not list:
            libs = [libs]
        for lib in libs:
            env.PrintDebugInfo('bindist', \
                "Ignoring libraries added using deprecated API: " + \
                os.path.join(os.path.relpath(os.curdir, \
                env.subst('${BUILD_ROOT}')), str(lib)))
    else:
        add_items_to_image(env, lib_list, targets, libs)

    return None

def add_objs_to_image(env, targets, objs):
    '''
    add_objs_to_image
    '''
    #import pdb; pdb.set_trace()
    root_env = env.get('ROOT_ENV')

    # get list container for current AU or global
    # if an AU container dosen't exist
    obj_list = root_env.get('AU_OBJS')
    if obj_list is None:
        obj_list = root_env.get('OBJS')

    if 'USES_BINARY_DIST' in env:
        for obj in objs:
            env.PrintDebugInfo('bindist', \
                "Ignoring objects added using deprecated API: " + \
                os.path.join(os.path.relpath(os.curdir, \
                env.subst('${BUILD_ROOT}')), str(obj)))
    else:
        add_items_to_image(env, obj_list, targets, objs)

    return None

def add_docs_to_image(env, targets, docs):
    '''
    add_docs_to_image
    '''
    #import pdb; pdb.set_trace()
    root_env = env.get('ROOT_ENV')

    # get list container for current AU or global
    # if an AU container dosen't exist
    doc_list = root_env.get('AU_DOCS')
    if doc_list is None:
        env.PrintError("NO AU_DOCS in root env..")
        raise RuntimeError('Invalid AU_DOCS variable')

    add_items_to_image(env, doc_list, targets, docs)
    return None

#------------------------------------------------------------------------------
# is Key or Target Enable
#------------------------------------------------------------------------------
def is_key_enable(env, keys):
    '''
    is_key_enable
    '''
    tgt_enable = False

    if type(keys) is not list:
        keys = [keys]

    # check that at least one of the targets will be build
    intersection = set(keys) & set(env.Dictionary())
    if len(intersection) > 0:
        tgt_enable = True

    return tgt_enable

#------------------------------------------------------------------------------
# Is the current build an internal or external build?
#------------------------------------------------------------------------------
def is_build_internal(env, pack_exception=None):
    '''
    is_build_internal

    Is the current build an internal or external build?
'''
    cleanpack = env.GetOption('cleanpack')
    pex = is_key_enable(env, pack_exception)
    return ((('USES_NO_STRIP_NO_ODM' in env) and \
        ('USES_CLEAN_PACK' not in env) and \
        not cleanpack) or pex)

# End of is_build_internal()

#------------------------------------------------------------------------------
# convert a list of source files with end with any of CPPSUFFIXES
# to a list of obj
#------------------------------------------------------------------------------
def convert_src_to_obj_list(env, list):
    '''
    convert_src_to_obj_list

    convert a list of source files with end with any of CPPSUFFIXES
    to a list of obj
'''
    cppsuffixes = env.get('CPPSUFFIXES')

    if cppsuffixes is not None:
        re_pattern_cppsuffixes = "|".join([re.escape(x) for x in cppsuffixes])
        re_pattern_cppsuffixes = re_pattern_cppsuffixes.replace(".", "")
        re_pattern_cppsuffixes = "^(.*)\.(%s)$" % re_pattern_cppsuffixes
        re_replace_cppsuffix = r"\1" + env.subst("${OBJSUFFIX}")

        list_obj = [re.sub(re_pattern_cppsuffixes, re_replace_cppsuffix, \
            str(source)) for source in list]
    else:
        list_obj = list

    return list_obj

#------------------------------------------------------------------------------
# check if item (library/object) is an exception to binary distribution
#------------------------------------------------------------------------------
def is_binary_exception(env, library):
    '''
    is_binary_exception

    check if item (library/object) is an exception to binary distribution
'''
    exceptions = env['BINEXCEPT_OPT']
    if exceptions:
        builddir = os.path.relpath(os.curdir, \
            env.subst('${BUILD_ROOT}')).replace(os.sep, '/')
        for exc in exceptions:
            if builddir.startswith(exc):
                return True
    return False

#------------------------------------------------------------------------------
# Add File wrapper for building env.File (optimizations)
#------------------------------------------------------------------------------
def add_file(env, targets, sources, use_file_list="LIBS", **args):
    '''
    add_file

    Add File wrapper for building env.File (optimizations)
'''
    file_nodes = None

    if is_key_enable(env, targets):

        if type(sources) is not list:
            sources = [sources]

        sources = env.Flatten(sources)

        file_nodes = [env.File(env.RealPath(str(src))) for src in sources]

        # now actually add lib
        root_env = env.get('ROOT_ENV')

        # get list container for current AU or global
        # if an AU container dosen't exist
        file_list = root_env.get("AU_" + use_file_list)

        if file_list is None:
            file_list = root_env.get("IMAGE_" + use_file_list)

            if file_list is None:
                file_list = root_env.get(use_file_list)

        add_nodes_to_list(env, file_list, file_nodes)

    return file_nodes

#------------------------------------------------------------------------------
# Add Library wrapper for building env.Library (optimizations)
#------------------------------------------------------------------------------
def AnalyzeLibrary(env, library, sources, libsuffix, pack=False, **args):
    '''
    AnalyzeLibrary()

    Analyze a library to determine how it should be built.  This function
    determines which location to use for the library, in the case of a
    Binary Distribution scenario, and which source files to use, if any,
    in a Packed Build.

    This function returns a structure with the following fields:

       * library:     A new library, to resolve any binary buildpath issues.
       * sources:     A new list of source files, which may be empty if this is
                      a packed build and no source files were shipped, or it
                      may contain a mixture of source and object files.
       * build_as:    Tells the caller whether to build this as a 'library',
                      or as just a 'file', which whould be the case if no
                      source files are shipped.

    If build_as is equal to 'file', the caller is expected to call env.File()
    with the library result.  If build_as is equal to 'library', the user
    should call env.Library() or similar function to create the library from
    the sources.

    The cleanpack function is handled internally.  The caller shouldn't have to
    do anything special to handle cleanpack.
    '''

    #------------------------
    # Return structure
    #------------------------

    class LibraryAnalysisData(object):
        """Results of analyzing a library for building in a packed build."""
        def __init__(self, library, sources):
            self.library = library
            self.sources = sources
            # Valid build_as values are 'library' and 'file'
            self.build_as = 'library'

    #------------------------
    # Initialization
    #------------------------

    if type(sources) is not list:
        sources = [sources]

    if pack:
        # make sure to use binary buildpath for binary deliverables,
        # OEM can change their build path, we need to use what was
        # originally used to build the binary node.
        library = str(library).replace("${BUILDPATH}", "${BINARY_BUILDPATH}")

    results = LibraryAnalysisData(library, env.Flatten(sources))

    #------------------------
    # Define control flags
    #------------------------

    # We're doing a cleanpack build
    do_cleanpack = (pack and
                    env.GetOption('cleanpack') and
                    not is_key_enable(env, args.get('PACK_EXCEPTION')))

    # We're in a packed build, using the gen_dep algorithm for determining what to build.
    use_gen_dep = "DEPENDENCY_TREE_DATA" in env

    # We're in a packed build, using the cleanpack algorithm for determining what to build.
    use_cleanpack = (pack and
                     not is_build_internal(env, args.get('PACK_EXCEPTION')) and
                     not use_gen_dep)

    #------------------------------
    # Do Gen Dependency Analysis
    #------------------------------

    if use_gen_dep:

        tgt_root = env.get('TARGET_ROOT').replace('\\', '/')
        lib_path = os.path.abspath(env.subst(results.library)).replace('\\', '/').replace(tgt_root, '')[1:] + ".lib"
        so_path = os.path.abspath(env.subst(results.library)).replace('\\', '/').replace(tgt_root, '')[1:] + ".so"
        if so_path in env["DEPENDENCY_TREE_DATA"]:
            tgt_path = so_path
        else:
            tgt_path = lib_path

        if env["DEPENDENCY_TREE_DATA"].has_key(tgt_path):
            if len(env["DEPENDENCY_TREE_DATA"][tgt_path]) == 0:
                results.sources = []
                results.build_as = 'file'
            else:
                if pack and not is_build_internal(env, args.get('PACK_EXCEPTION')):
                    # This is a hack since it's dependent on us being called by
                    # env.AddBinaryLibrary() the pack exception flags, and the
                    # whole purpose of the gen dependency mechanism is to get away
                    # from this dependency.  I believe the real fix will be to
                    # include source files in the dependency tree, and then remove
                    # source files from results.sources if their object file is
                    # present in the dependency tree, but their source file is
                    # absent.  In fact, the .o file doesn't need to be in the
                    # dependency tree if the .c file is.  This means that the
                    # dependency tree files don't need to be any larger.
                    results.sources = []
                    results.build_as = 'file'

                for value in env["DEPENDENCY_TREE_DATA"][tgt_path]:
                    if env["DEPENDENCY_TREE_DATA"].has_key(value):
                        # add *.o file to sources
                        # may also need to replace erroneous *.c file
                        index = value.find(env.subst("${BUILDPATH}"))
                        if index >= 0:
                            results.sources.append(value[index:])

            # End of else len(DEPENDENCY_TREE_DATA) > 0
        # End of if DEPENDENCY_TREE_DATA has tgt_path
    # End of if use_gen_dep

    #-----------------------------------
    # Do Binary Distribution Analysis
    #-----------------------------------

    if 'USES_BINARY_DIST' in env:
        sconsdir = os.path.relpath(os.curdir, \
            env['BUILD_ROOT']).replace(os.sep, '/')
        if is_binary_exception(env, sconsdir + '/' + library):
            # normal case, regular library
            env.PrintDebugInfo('bindist', "Binary dist exception for: " + \
                sconsdir + '/' + results.library)
        else:
            # no exception, so use binary distribution
            env.PrintDebugInfo('bindist', "Binary dist for: " + \
                sconsdir + '/' + results.library)
            results.sources = []
            results.build_as = 'file'

    elif do_cleanpack or use_cleanpack:

        #-------------------------
        # Do cleanpack analysis
        #-------------------------

        saved_sources = results.sources  # Save this for later
        results.sources = []
        results.build_as = 'file'

        if do_cleanpack:

            library_node = env.Library(results.library+"_cleanpack", saved_sources)

            if ((type(library_node) is list) or
                (type(library_node) is SCons.Node.NodeList)):
                library_node = library_node[0]

            no_exec = env.GetOption('no_exec')

            if no_exec or env.PathExists(library+libsuffix):
                env.PrintInfo("CleanPack binary library [%s]" % library_node.path)
                for src in saved_sources:
                    node = env.RealPath(str(env.GetBuildPath(src)))
                    env.PrintInfo("CleanPack for library [%s] will " \
                        "remove file [%s]" % (library_node.name, node))

                    if not no_exec:
                        # make sure files are writeable
                        # so they can be removed
                        if env.PathExists(node):
                            os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | \
                                stat.S_IRWXO)

                    # add to SCons clean list
                    env.Clean(library_node, node)

            # End of if no_exec or library exists
            else:
                # Error condition:  Not no_exec, but library doesn't exist
                env.PrintError("---------------------------------------" \
                    "----------------------------------------")
                env.PrintError("Required binary library missing -->")
                env.PrintError("  [%s]" % \
                    env.RealPath(results.library+libsuffix))
                env.PrintError("Image name [%s]" % \
                    env.get('IAMGE_NAME', "Unknown"))
                env.PrintError("Lib name from script [%s]" % \
                    env.subst(library))
                env.PrintError("CleanPack processing will not " \
                    "be performed")
                env.PrintError("---------------------------------------" \
                    "----------------------------------------")

                # Set an error level that can be checked and exited on
                # later after all add_library calls are done check
                # keep-on-going flag and exit only if not set
                Exit(1)

        # End of else do_cleanpack

    # End of else use_cleanpack or do_cleanpack

    # If we should build as 'file', do subst and add file suffix to the
    # library, since the env.File() builder doesn't do this.

    if results.build_as == 'file':
        if not results.library.endswith(libsuffix):
            results.library += libsuffix
        results.library = env.subst(results.library)

    # Return our results
    return results

# End of AnalyzeLibrary()

#------------------------------------------------------------------------------
# Add Library wrapper for building env.Library (optimizations)
#------------------------------------------------------------------------------
def add_library_not_ready_yet(env, targets, library, sources, pack=False, \
    add_to_list=True, **args):
    '''
    add_library

    Add Library wrapper for building env.Library (optimizations)
    '''
    library_node = None

    if is_key_enable(env, targets):

        analysis = env.AnalyzeLibrary(library, sources, env['LIBSUFFIX'], pack, **args)

        if analysis.build_as == 'library':
            library_node = env.Library(analysis.library, analysis.sources)
        else:
            library_node = env.File(analysis.library)

        # add node(s) to appropiate list (bucket)
        if add_to_list:
            # now actually add lib
            root_env = env.get('ROOT_ENV')

            # get list container for current AU or global
            # if an AU container dosen't exist
            lib_list = root_env.get('AU_LIBS')

            if lib_list is None:
                lib_list = root_env.get('IMAGE_LIBS')

                if lib_list is None:
                    lib_list = root_env.get('LIBS')

            add_nodes_to_list(env, lib_list, library_node)

    return library_node

# End of add_library()

def add_library(env, targets, library, sources, pack=False, \
    add_to_list=True, **args):
    '''
    add_library

    Add Library wrapper for building env.Library (optimizations)
    '''
    library_node = None

    if is_key_enable(env, targets):

        if type(sources) is not list:
            sources = [sources]

        sources = env.Flatten(sources)
        
        check_ddp_libpath = False
        library_conv = str(library).replace("${BUILDPATH}", "${BINARY_BUILDPATH}")
        tgt_root = env.get('TARGET_ROOT')
        libpath = os.path.abspath(env.subst(library_conv)).replace(os.sep, '/').replace(tgt_root, '')[1:] + ".lib"

        if "DEPENDENCY_TREE_DATA" in env and \
            env["DEPENDENCY_TREE_DATA"].has_key(libpath):
            if len(env["DEPENDENCY_TREE_DATA"][libpath]) == 0:
                check_ddp_libpath = True
                pack = True
            else:
                for value in env["DEPENDENCY_TREE_DATA"][libpath]:
                    if env["DEPENDENCY_TREE_DATA"].has_key(value):
                        # add *.o file to sources
                        # may also need to replace erroneous *.c file
                        index = value.find(env.subst("${BUILDPATH}"))
                        if index >= 0:
                            sources.append(value[index:])

        if 'USES_BINARY_DIST' in env:
            sconsdir = os.path.relpath(os.curdir, \
                env['BUILD_ROOT']).replace(os.sep, '/')
            if pack:
                library = str(library).replace("${BUILDPATH}", \
                    "${BINARY_BUILDPATH}")
            if is_binary_exception(env, sconsdir + '/' + library):
                # normal case, regular library
                env.PrintDebugInfo('bindist', "Binary dist exception for: " + \
                    sconsdir + '/' + library)
                library_node = env.Library(library, sources)
            if not library_node:
                # no exception, so use binary distribution
                # check if libsuffix is present
                if not library.endswith(env.subst("${LIBSUFFIX}")):
                    library += "${LIBSUFFIX}"
                env.PrintDebugInfo('bindist', "Binary dist for: " + \
                    sconsdir + '/' + library)
                library_node = env.File(env.subst(library))

        elif pack:
            # make sure to use binary buildpath for binary deliverables,
            # OEM can change their build path, we need to use what was
            # originally used to build the binary node.
            library = str(library).replace("${BUILDPATH}", \
                "${BINARY_BUILDPATH}")

            cleanpack = env.GetOption('cleanpack')

            pack_exception = is_key_enable(env, args.get('PACK_EXCEPTION'))

            if check_ddp_libpath:
                library_node = env.File(env.subst(library+"${LIBSUFFIX}"))
            elif is_build_internal(env, args.get('PACK_EXCEPTION')):
                library_node = env.Library(library, sources)
            elif cleanpack:
                library_node = env.Library(library+"_cleanpack", sources)
            else:
                library_node = env.File(env.subst(library+"${LIBSUFFIX}"))

            if cleanpack and not pack_exception:
                if (type(library_node) is list) or \
                    (type(library_node) is SCons.Node.NodeList):
                    library_node = library_node[0]

                no_exec = env.GetOption('no_exec')

                if no_exec or env.PathExists(library+"${LIBSUFFIX}"):
                    env.PrintInfo("CleanPack binary library [%s]" % \
                        library_node.path)
                    for src in sources:
                        node = env.RealPath(str(env.GetBuildPath(src)))
                        env.PrintInfo("CleanPack for library [%s] will " \
                            "remove file [%s]" % (library_node.name, node))

                        if not no_exec:
                            # make sure files are writeable
                            # so they can be removed
                            if env.PathExists(node):
                                os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | \
                                    stat.S_IRWXO)

                            # remove file
                            #os.unlink(env.RealPath(str(node)))
                            # cannot remove file here since SCons may have this
                            # files open for dependency analysts resulting in
                            # OS errors as cannot delete file is being used by
                            # another program.

                        # add to SCons clena list
                        env.Clean(library_node, node)
                else:
                    env.PrintError("---------------------------------------" \
                        "----------------------------------------")
                    env.PrintError("Required binary library missing -->")
                    env.PrintError("  [%s]" % \
                        env.RealPath(library+"${LIBSUFFIX}"))
                    env.PrintError("Image name [%s]" % \
                        env.get('IAMGE_NAME', "Unknown"))
                    env.PrintError("Lib name from script [%s]" % \
                        env.subst(library))
                    env.PrintError("CleanPack processing will not " \
                        "be performed")
                    env.PrintError("---------------------------------------" \
                        "----------------------------------------")

                    # Set an error level that can be checked and exited on
                    # later after all add_library calls are done check
                    # keep-on-going flag and exit only if not set
                    Exit(1)

        else:
            # normal case, regular library
            library_node = env.Library(library, sources)

        # add node(s) to appropiate list (bucket)
        if add_to_list:
            # now actually add lib
            root_env = env.get('ROOT_ENV')

            # get list container for current AU or global
            # if an AU container dosen't exist
            lib_list = root_env.get('AU_LIBS')

            if lib_list is None:
                lib_list = root_env.get('IMAGE_LIBS')

                if lib_list is None:
                    lib_list = root_env.get('LIBS')

            add_nodes_to_list(env, lib_list, library_node)

    return library_node

def add_bin_library(env, targets, library, sources, \
    pack_exception=None, **kargs):
    '''
    add_bin_library
    '''
    return add_library(env, targets, library, sources, pack=True, \
        PACK_EXCEPTION=pack_exception, **kargs)

def add_oem_library(env, targets, library, **kargs):
    '''
    add_oem_library
    '''
    return add_file(env, targets, library, use_file_list="LIBS", **kargs)

#------------------------------------------------------------------------------
# Add Object wrapper for building env.Object (optimizations)
#------------------------------------------------------------------------------
def add_object(env, targets, sources, pack=False, add_to_list=True, **args):
    '''
    add_object

    Add Object wrapper for building env.Object (optimizations)
    '''
    object_nodes = []

    if is_key_enable(env, targets):

        if type(sources) is not list:
            sources = [sources]

        sources = env.Flatten(sources)

        # We're in a packed build, using the gen_dep algorithm for determining what to build.
        # (Forget about pack exceptions for use_gen_dep since that's a cleanpack thing.)
        use_gen_dep = ("DEPENDENCY_TREE_DATA" in env and
                       not is_build_internal(env) and
                       not env.GetOption('cleanpack'))

        check_ddp_objpath = False
        obj_paths = []
#        if use_gen_dep:
        if "DEPENDENCY_TREE_DATA" in env:
            sources_obj = convert_src_to_obj_list(env, sources)
            for source_obj in sources_obj:
                tgt_root = env.get('TARGET_ROOT')
                obj_path = env.RealPath(str(env.GetBuildPath(source_obj))).replace(os.sep, '/').replace(tgt_root, '')[1:]
                if env["DEPENDENCY_TREE_DATA"].has_key(obj_path):
                    obj_paths.append(env.RealPath(source_obj))
                    check_ddp_objpath = True
                    pack = True
            if not check_ddp_objpath:
                object_nodes = env.Object(sources)

        if 'USES_BINARY_DIST' in env:
            sconsdir = os.path.relpath(os.curdir, \
                env['BUILD_ROOT']).replace(os.sep, '/')
            if pack:
                sources = [str(source).replace("${BUILDPATH}", \
                    "${BINARY_BUILDPATH}") for source in sources]
            sources_obj = convert_src_to_obj_list(env, sources)
            for obj in sources_obj:
                if is_binary_exception(env, sconsdir + '/' + obj):
                    # normal case, regular object
                    env.PrintDebugInfo('bindist', \
                        "Binary dist exception for: " + sconsdir + '/' + obj)
                    object_nodes.append(env.Object(obj))
                else:
                    # no exception, so use binary distribution
                    # check if libsuffix is present
                    if not obj.endswith(env.subst("${OBJSUFFIX}")):
                        env.PrintError \
                            ("Object doesn't end in correct suffix: " + obj)
                    env.PrintDebugInfo('bindist', "Binary dist for: " + \
                        sconsdir + '/' + obj)
                    object_nodes.append(env.File(env.subst(obj)))

        elif pack:
            # make sure to use binary buildpath for binary deliverables,
            # OEM can change their build path, we need to use what was
            # originally used to build the binary node.
            sources = [str(source).replace("${BUILDPATH}", \
                "${BINARY_BUILDPATH}") for source in sources]

            cleanpack = env.GetOption('cleanpack')

            pack_exception = is_key_enable(env, args.get('PACK_EXCEPTION'))

            if check_ddp_objpath:
                for obj_path in obj_paths:
                    object_nodes.append(env.File(obj_path))
            elif is_build_internal(env, args.get('PACK_EXCEPTION')):
                object_nodes = env.Object(sources)
            elif cleanpack:
                cp_sources = []
                for source in sources:
                    src_path, src_file = os.path.split(source)
                    cp_sources.append("".join \
                        ([src_path, "/cleanpack/", src_file]))

                object_nodes = env.Object(cp_sources)
            else:
                sources_obj = convert_src_to_obj_list(env, sources)
                object_nodes = \
                    [env.File(env.RealPath(obj)) for obj in sources_obj]
            
            if cleanpack and not pack_exception:
                no_exec = env.GetOption('no_exec')

                if not no_exec:
                    # check binary objects exit before we delete the sources
                    sources_obj = convert_src_to_obj_list(env, sources)

                    for obj in sources_obj:
                        if not env.PathExists(str(obj)):
                            env.PrintError("-------------------------------" \
                                "-------------------------------------------" \
                                "-----")
                            env.PrintError("Required binary object " \
                                "missing -->")
                            env.PrintError("  [%s]" % env.RealPath(str(obj)))
                            env.PrintError("Image name [%s]" % \
                                env.get('IAMGE_NAME', "Unknown"))
                            env.PrintError("Object name from script [%s]" % \
                                env.subst(str(obj)))
                            env.PrintError("CleanPack processing will not " \
                                "be performed")
                            env.PrintError("-------------------------------" \
                                "-------------------------------------------" \
                                "-----")

                            # Set an error level that can be checked and exited
                            # on later after all add_library calls are done
                            # check keep-on-going flag and exit only if not set
                            Exit(1)

                for src in sources:
                    node = env.RealPath(str(env.GetBuildPath(src)))
                    env.PrintInfo("CleanPack will remove file [%s]" % node)

                    if not no_exec:
                        # make sure files are writeable so they can be removed
                        if env.PathExists(node):
                            os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | \
                                stat.S_IRWXO)

                        # remove file
                        #os.unlink(env.RealPath(str(node)))
                        # cannot remove file here since SCons may have this
                        # files open for dependency analysts resulting in OS
                        # errors as cannot delete file
                        # is being used by another program.

                    # add to SCons clean list
                    env.Clean(object_nodes, node)
        else:
            # normal case, regular object
            object_nodes = env.Object(sources)

        # add node(s) to appropiate list (bucket)
        if add_to_list:
            # now actually add lib
            root_env = env.get('ROOT_ENV')

            # get list container for current AU or global
            # if an AU container dosen't exist
            obj_list = root_env.get('AU_OBJS')

            if obj_list is None:
                obj_list = root_env.get('IMAGE_OBJS')

                if obj_list is None:
                    obj_list = root_env.get('OBJS')

            add_nodes_to_list(env, obj_list, object_nodes)

    return object_nodes

def add_bin_object(env, targets, sources, pack_exception=None, **kargs):
    '''
    add_bin_object
    '''
    return add_object(env, targets, sources, pack=True, \
        PACK_EXCEPTION=pack_exception, **kargs)

def add_oem_object(env, targets, object, **kargs):
    '''
    add_oem_object
    '''
    return add_file(env, targets, object, use_file_list="OBJS", **kargs)

#------------------------------------------------------------------------------
# Add Document wrapper for building env.Document
#------------------------------------------------------------------------------
def add_document(env, targets, document, sources, **args):
    '''
    add_document

    Add Document wrapper for building env.Document
    '''
    document_nodes = None

    if is_key_enable(env, targets):
        document_nodes = env.PdfDocument(document, sources)

        # now actually add lib
        root_env = env.get('ROOT_ENV')

        # get list container for current AU or global
        # if an AU container dosen't exist
        doc_list = root_env.get('AU_DOCS')

        if doc_list is None:
            doc_list = root_env.get('IMAGE_DOCS')

            if doc_list is None:
                env.PrintError("NO AU_DOCS or IMAGE_DOCS in root env..")
                raise RuntimeError \
                    ('Invalid or missing AU_DOCS, IMAGE_DOCS variables')

        add_nodes_to_list(env, doc_list, document_nodes)

    return document_nodes

#------------------------------------------------------------------------------
# Add list files (link nodes listed in a text file) pass the linker
# for example ARM uses --via.
#------------------------------------------------------------------------------
def add_listfile(env, targets, sources, **args):
    '''
    add_listfile

    Add list files (link nodes listed in a text file) pass the linker
    for example ARM uses --via.
    '''
    listfile_node = None

    if is_key_enable(env, targets):

        if type(sources) is not list:
            sources = [sources]

        listfile_node = env.Flatten(sources)

        # now actually add lib
        root_env = env.get('ROOT_ENV')

        # get list container for current AU or global
        # if an AU container dosen't exist
        listfile_list = root_env.get('AU_LISTFILES')

        if listfile_list is None:
            listfile_list = root_env.get('IMAGE_LISTFILES')

            if listfile_list is None:
                env.PrintError \
                    ("NO AU_LISTFILES or IMAGE_LISTFILES in root env..")
                raise RuntimeError('Invalid or missing AU_LISTFILES, ' \
                    'IMAGE_LISTFILES variables')

        add_nodes_to_list(env, listfile_list, listfile_node)

        # list files are special case since we cannot added directly as a
        # dependency of the Program. For a correct Solution we might have
        # to add our own AddProgram Wrapper. In the mean time add an alias
        # form the aliases to this node.
        #aliases = env.get('IMAGE_ALIASES')
        #if aliases is not None:
        #    [env.Alias(alias, node) \
        #        for alias in aliases for node in listfile_node]

    return listfile_node

#------------------------------------------------------------------------------
# Add Artefact (Non-Link Node) for example scripts, etc.
#------------------------------------------------------------------------------
def add_artifact(env, targets, sources, **args):
    '''
    add_artifact

    Add Artefact (Non-Link Node) for example scripts, etc.
    '''
    artifact_node = None

    if is_key_enable(env, targets):

        if type(sources) is not list:
            sources = [sources]

        artifact_node = env.Flatten(sources)

        # now actually add lib
        root_env = env.get('ROOT_ENV')

        # get list container for current AU or global
        # if an AU container dosen't exist
        artifacts_list = root_env.get('AU_ARTIFACTS')

        if artifacts_list is None:
            artifacts_list = root_env.get('IMAGE_ARTIFACTS')

            if artifacts_list is None:
                env.PrintError \
                    ("NO AU_ARTIFACTS or IMAGE_ARTIFACTS in root env..")
                raise RuntimeError('Invalid or missing AU_ARTIFACTS, ' \
                    'IMAGE_ARTIFACTS variables')

        add_nodes_to_list(env, artifacts_list, artifact_node)

        # artifacts are special case since we cannot added directly as a
        # dependency of the Program. For a correct Solution we might have
        # to add our own AddProgram Wrapper. In the mean time add an alias
        # form the aliases to this node.
        aliases = env.get('IMAGE_ALIASES')
        if aliases is not None:
            [env.Alias(alias, node) \
                for alias in aliases for node in artifact_node]

    return artifact_node

#------------------------------------------------------------------------------
# clean_pack - Remove the pack files during the clean process
#------------------------------------------------------------------------------
def clean_pack(env, targets, sources, **args):
    '''
    clean_pack

    Remove the pack files during the clean process
    '''
    if is_key_enable(env, targets):
        cleanpack = env.GetOption('cleanpack')
        no_exec = env.GetOption('no_exec')

        pack_exception = (is_key_enable(env, args.get('PACK_EXCEPTION')) or
                          is_key_enable(env, args.get('pack_exception')))

        if cleanpack and not pack_exception:

            if type(sources) is not list:
                sources = [sources]

            # create a dummy builder just for this clean action
            env_cleanpack = env.Clone()

            def cleanpack_fake_action(target, source, env):
                '''
                cleanpack_fake_action
                '''
                # fake function to use command builder
                return None

            # use like
            #env_cleanpack.Command('foo.out', 'foo.in', cleanpack_fake_action)

            aliases = env_cleanpack.get('IMAGE_ALIASES')

            for src in sources:
                node = env.RealPath(str(env_cleanpack.GetBuildPath(src)))
                env_cleanpack.PrintInfo \
                    ("CleanPack will remove file [%s]" % node)

                if not no_exec:
                    # make sure files are writeable so they can be removed
                    if env_cleanpack.PathExists(node):
                        os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | \
                            stat.S_IRWXO)

                    # remove file
                    #if not no_exec:
                    #   os.unlink(env_cleanpack.RealPath(str(node)))
                    # cannot remove file here since SCons may have this files
                    # open for dependency analysts resulting in OS errors as
                    # cannot delete file is being used by another program.

                # add to SCons clean list
                cp_node = env_cleanpack.Command \
                    (node, None, cleanpack_fake_action)

                # cleanpack nodes are special case since we cannot directly
                # connect to a target for now aliases this node to current
                # aliases if aliases is not None:
                [env_cleanpack.Alias(alias, cp_node) for alias in aliases]


    return None

#------------------------------------------------------------------------------
# Add Program (Link into exe, elf, etc).
#------------------------------------------------------------------------------
def add_program(env, target=None, source=None, **kargs):
    '''
    add_program

    Add Program (Link into exe, elf, etc).
    '''
    env.PrintError('')
    env.PrintError("--------------------------------------------------------" \
        "-----------------------")
    env.PrintError("This version of AddProgram is deprecated, please " \
        "contact the SCons arch team")
    env.PrintError("for further assistance. The tool sets should define the " \
        "AddProgram method.")
    env.PrintError("--------------------------------------------------------" \
        "-----------------------")
    env.PrintError('')
    Exit(1)

#==============================================================================
# check_alias
# Check if script should be loaded depending on alias, also used to list
# the target name and aliases
#==============================================================================
def check_alias(env, image_name="${IMAGE_NAME}", \
    alias_list=None, always_true=False):
    '''
    check_alias

    Check if script should be loaded depending on alias, also used to list
    the target name and aliases
    '''

    # init vars
    alias_math = False
    cmdline_targets = map(str, COMMAND_LINE_TARGETS)

    listtargets = env.GetOption('listtargets')
    listapis = env.GetOption('listapis')
    help_option = env.GetOption('help')

    if alias_list is None:
        alias_list = env.get('IMAGE_ALIASES')
    elif type(alias_list) is not list:
        alias_list = [alias_list]

    if listtargets or listapis or help_option:
        # if help - print short image info (also overide all others)
        # if listtargets - print detail info per image
        # if listapis - do nothing
        if help_option:
            print string.ljust(env.subst(image_name + " ${PROC} "), 22) + \
                str(alias_list)
        elif listtargets:
            if len(cmdline_targets) == 0 or \
                len(set(cmdline_targets) & set(alias_list)) > 0:
                wrapper = textwrap.TextWrapper()
                wrapper.width = 80
                wrapper.subsequent_indent = "     "

                print " "
                print "IMAGE: %s" % env.subst(image_name)

                # proc info
                wrapper.initial_indent = "  proc:        "
                for print_line in wrapper.wrap(env.subst("${PROC}")):
                    print print_line

                wrapper.initial_indent = "  proc config: "
                for print_line in wrapper.wrap(env.subst("${PROC_CONFIG}")):
                    print print_line

                # print aliases
                wrapper.initial_indent = "  aliases:     "
                for print_line in wrapper.wrap \
                    (env.subst("${IMAGE_ALIASES}").replace(" ", ", ")):
                    print print_line

                # print build tags
                wrapper.initial_indent = "  build tags:  "
                for print_line in wrapper.wrap \
                    (env.subst("${BUILD_TAGS}").replace(" ", ", ")):
                    print print_line

                # print deprecated build tags
                wrapper.initial_indent = "  deprecated build tags: "
                for print_line in wrapper.wrap \
                    (env.subst("${DEPRECATED_BUILD_TAGS}").replace(" ", ", ")):
                    print print_line

                # print image path location
                wrapper.initial_indent = "  image path:  "
                path_from_root = "".join(["${BUILD_ROOT}", \
                    env.RealPath(inspect.stack()[2][1], posix=True).replace \
                    (env.RealPath('${BUILD_ROOT}', posix=True), "")])
                for print_line in wrapper.wrap(path_from_root):
                    print print_line

        return alias_math

    if len(cmdline_targets) > 0:
        for target in cmdline_targets:
            if ('\\' in target) or ('/' in target):
                if str(target).find(alias_list[0]) != -1:
                    alias_math = True
                    break
            elif target in alias_list:
                alias_math = True
                break

    if always_true:
        filter_opt = env.get('FILTER_OPT')
        if filter_opt is not None and not alias_math:
            alias_math = False
        else:
            alias_math = True

    # last but not least print image banner (only if alias matches).
    if alias_math:
        env.PrintImageBanner(image_name)

    return alias_math

#==============================================================================
# bind_aliases_to_targets
# utility to bind image targets to image aliases
#==============================================================================
def bind_aliases_to_targets(env, target, alias_list=None):
    '''
    bind_aliases_to_targets

    utility to bind image targets to image aliases
    '''
    if alias_list is None:
        alias_list = env.get('IMAGE_ALIASES')
    elif type(alias_list) is not list:
        alias_list = [alias_list]

    for alias in alias_list:
        env.Alias(alias, target)

#==============================================================================
# print_stage_banner
# print standard stage
#==============================================================================
def print_stage_banner(env, stage_name):
    '''
    print_stage_banner

    print standard stage
    '''
    print "\n"
    print "-----------------------------------------------------------------" \
        "-------------"
    print "   %s" % str(stage_name)
    print "-----------------------------------------------------------------" \
        "-------------"

#==============================================================================
# print_image_banner
# print standard image banner
#==============================================================================
def print_image_banner(env, image_name="${IMAGE_NAME}"):
    '''
    print_image_banner

    print standard image banner
    '''
    print string.ljust(env.subst(image_name + " ${PROC}"), 22) + \
        env.subst("BUILD_ID=${BUILD_ID}, TOOL_ENV=${PLATFORM}")

#==============================================================================
# print_error
# print standard error message
#==============================================================================
def print_error(env, message):
    '''
    print_error

    print standard error message
    '''
    print '!! ERROR: ' + env.subst(str(message))

#==============================================================================
# print_warning
# print standard warning message
#==============================================================================
def print_warning(env, message, width=80, raw=True):
    '''
    print_warning

    print standard warning message
    '''
    #wrapper = textwrap.TextWrapper()
    #wrapper.width = width
    #wrapper.initial_indent =    "!! WARNING: "
    #wrapper.subsequent_indent = "   "
    #for print_line in wrapper.wrap(env.subst(str(message))):
    #    print print_line

    if not env.get('DISABLE_BUILD_WARNINGS', False):
        print '!! WARNING: ' + env.subst(str(message), raw)
        sys.stdout.flush()

#==============================================================================
# print_info
# print standard info message
#==============================================================================
def print_info(env, message, raw=True):
    '''
    print_info

    print standard info message
    '''
    print '** INFO: ' + env.subst(str(message), raw)

#==============================================================================
# print_debug_info
# print standard debug info message, depeding on debug info type
#==============================================================================
def print_debug_info(env, debuginfo_type, message, raw=True):
    '''
    print_debug_info

    print standard debug info message, depeding on debug info type
    '''
    debuginfo_type = str(debuginfo_type)
    debuginfo_opt = env.get('DEBUGINFO_OPT')

    if debuginfo_type in debuginfo_opt:
        print '** DBGINF: ' + env.subst(str(message), raw)

#==============================================================================
# print_data
# print standard info message
#==============================================================================
def print_data(env, message, raw=True):
    '''
    print_data

    print standard info message
    '''
    print env.subst(str(message), raw)

#==============================================================================
# create_file_banner
# creates a string that can be use as a banner for auto generated files
#==============================================================================
def create_file_banner(env, fname, description="None", start_comment="#", \
    end_comment="", start_block="", end_block="", style='none'):
    '''
    create_file_banner

    creates a string that can be use as a banner for auto generated files
    '''
    banner_str = \
'''$SB$SCM================================================================''' \
'''============$ECM
$SCM  Name:                                                               ''' \
'''      $ECM
$SCM    $FILE_NAME $ECM
$SCM
$SCM  Description:                                                        ''' \
'''      $ECM
$SCM    $DESCRIPTION $ECM
$SCM                                                                      ''' \
'''      $ECM
$SCM Copyright (c) $YEAR by QUALCOMM Technologies, Incorporated.  All Rights Reserved. ''' \
'''       $ECM
$SCM======================================================================''' \
'''======$ECM
$SCM                                                                      ''' \
'''      $ECM
$SCM *** AUTO GENERATED FILE - DO NOT EDIT                                ''' \
'''      $ECM
$SCM                                                                      ''' \
'''      $ECM
$SCM GENERATED: $DATE $ECM
$SCM======================================================================''' \
'''======$ECM$EB
'''
    banner_single_str = "*** DO NOT EDIT - AUTO GENERATED FILE ($DATE), Name:"\
        "$FILE_NAME, Description:$DESCRIPTION, Copyright (c) $YEAR by " \
        "QUALCOMM Technologies, Incorporated.  All Rights Reserved."

    banner_json_str = "{' ***':'DO NOT EDIT - AUTO GENERATED FILE','Name':" \
        "'$FILE_NAME','Description':'$DESCRIPTION','Generated':'$DATE',"\
        "'Copyright':'Copyright (c) $YEAR by QUALCOMM Technologies, Incorporated.  " \
        "All Rights Reserved.'}"

    if style == 'C':
        start_comment = "#"
        end_comment = ""
        start_block = "/*\n"
        end_block = "\n*/"

    elif style == 'C++':
        start_comment = "//"
        end_comment = ""
        start_block = ""
        end_block = ""

    elif style == 'asm':
        start_comment = ";"
        end_comment = ""
        start_block = ""
        end_block = ""

    elif style == 'make' or style == 'shell':
        start_comment = "#"
        end_comment = ""
        start_block = ""
        end_block = ""

    elif style == 'dos':
        start_comment = "REM "
        end_comment = ""
        start_block = ""
        end_block = ""
    elif style == 'single-line':
        banner_str = banner_single_str
    elif style == 'json':
        banner_str = banner_json_str

    banner_str = banner_str.replace('$SCM', start_comment)
    banner_str = banner_str.replace('$ECM', end_comment)
    banner_str = banner_str.replace('$SB', start_block)
    banner_str = banner_str.replace('$EB', end_block)
    banner_str = banner_str.replace('$YEAR', \
        str(datetime.now().strftime('%Y')))
    banner_str = banner_str.replace('$DATE', str(datetime.now().ctime()))
    banner_str = banner_str.replace('$FILE_NAME', fname)
    banner_str = banner_str.replace('$DESCRIPTION', description)

    return banner_str


#==============================================================================
# real_path
#==============================================================================
def real_path(env, paths, posix=False):
    '''
    real_path
    '''
    env_subst = env.subst
    ospath_realpath = os.path.realpath

    #import pdb; pdb.set_trace()
    if env.get('PLATFORM', 'none') == 'cygwin':
        posix = True

    if type(paths) is list:
        paths = [env_subst(path) for path in paths]

        for index in range(len(paths)):
            if (not os.path.isabs(paths[index])):
                tmp_path = os.path.join(env_subst('${TARGET_ROOT}'), paths[index])
                if (not os.path.exists(ospath_realpath(paths[index]))) and (os.path.exists(tmp_path)) and (os.path.isabs(tmp_path)):
                    paths[index] = tmp_path

            paths[index] = ospath_realpath(paths[index])

        if posix:
            paths = [path.replace('\\', '/') for path in paths]
    else:
        paths = env_subst(paths)

        if(not os.path.isabs(paths)):
            tmp_path = os.path.join(env_subst('${TARGET_ROOT}'), paths)
            if (not os.path.exists(ospath_realpath(paths))) and (os.path.exists(tmp_path)) and (os.path.isabs(tmp_path)):
                paths = tmp_path

        paths = ospath_realpath(paths)

        if posix:
            paths = paths.replace('\\', '/')

    return paths


#==============================================================================
# norm_path
#==============================================================================
def norm_path(env, paths, posix=False):
    '''
    norm_path
    '''
    env_subst = env.subst
    ospath_normpath = os.path.normpath

    if env.get('PLATFORM', 'none') == 'cygwin':
        posix = True

    if type(paths) is list:
        paths = [env_subst(path) for path in paths]
        paths = [ospath_normpath(path) for path in paths]

        if posix:
            paths = [path.replace('\\', '/') for path in paths]
    else:
        paths = env_subst(paths)
        paths = ospath_normpath(paths)

        if posix:
            paths = paths.replace('\\', '/')

    return paths

#==============================================================================
# path_exists
# Substitue envirometn variables and check if path exists
#==============================================================================
def path_exists(env, path):
    '''
    path_exists

    Substitue envirometn variables and check if path exists
    '''
    path = env.subst(path)
    return os.path.exists(path)

#==============================================================================
# path_to_dict_tree
# Converst a list of paths into a dictionary used as a tree data structure.
# leafs point to empty dictionaries
#==============================================================================
def path_to_dict_tree(env, path_list, path_sep='/'):
    '''
    path_to_dict_tree

    Converst a list of paths into a dictionary used as a tree data structure.
    leafs point to empty dictionaries
    '''
    dict_tree = dict()

    if type(path_list) is not list:
        path_list = [path_list]

    # iterate filters and decompose each filter into its parts
    for path in path_list:
        path_parts = path.split(path_sep)

        # now create tree structure from the filter parts
        current_dict = dict_tree
        for part in path_parts:
            dict_key = part
            sub_dict = current_dict.get(dict_key)

            # if key is not present create new entry
            if sub_dict is None:
                current_dict[dict_key] = dict()
                sub_dict = current_dict.get(dict_key)

            current_dict = sub_dict

    return dict_tree
#==============================================================================
# filter_match
#
#==============================================================================
def filter_match(env, path=""):
    '''
    filter_match
    '''
    found_match = False
    filter_opt = env.get('FILTER_OPT')

    if filter_opt is not None and len(path) > 0:
        # for the most part filters are part of the path after ${BUILD_ROOT}
        # but since this is configurable it is not guranteed. So for speed and
        # sanity purposes, the only job is to see if we can find a filter
        # within the provided path. Filter should be posix like so make sure
        # our path is also posix like
        path = env.NormPath(path, posix=True)
        for filter_item in filter_opt:
            if path.find(filter_item) != -1:
                found_match = True
                break

    return found_match


#==============================================================================
# DeleteAction / get_delete_action
# return an action that can be used to delete files when a builder call it's
# actions, also follows the general text formatting rules
#==============================================================================
class DeleteAction(object):
    '''
    class DeleteAction

    return an action that can be used to delete files when a builder call it's
    actions, also follows the general text formatting rules
    '''
    def __init__(self, file):
        '''
        __init__
        '''
        self.file = file
        self.verbose = int(GetOption('verbose'))

    def delete(self, target, source, env):
        '''
        delete
        '''
        file_item = self.file

        if '${TARGET}' in self.file:
            file_item = file_item.replace('${TARGET}', target[0].path)
        elif '$TARGET' in self.file:
            file_item = file_item.replace('$TARGET', target[0].path)

        #import pdb; pdb.set_trace()

        if self.file is not None:
            if os.path.exists(file_item):
                os.unlink(file_item)
            else:
                print "*** Can't delete file: " + file_item

        return 0

def get_delete_action(env, file):
    '''
    get_delete_action
    '''
    verbose = int(GetOption('verbose'))

    if verbose >= 2:
        verbose_str = "<== Deleting \nFile: "+ file +"\n==>"
    elif verbose == 1:
        verbose_str = "=== Deleting   "+ file
    else:
        verbose_str = "=== Deleting   ${BUILDPATH}/" + \
            file.replace('TARGET', 'TARGET.name')

    dact = DeleteAction(file)
    delete_act = SCons.Action.Action(dact.delete, verbose_str)

    return delete_act

#==============================================================================
# Run CMDS
# Runs commands passed in from builders, and respects the verbose levels.
# (deprecated, use ExecCmds, this functions take parameters that are not used
# bt must be passed in)
#==============================================================================
def run_cmds(env, target, source, cmds, dir=None, silent=None):
    '''
    run_cmds

    Runs commands passed in from builders, and respects the verbose levels.
    (deprecated, use ExecCmds, this functions take parameters that are not used
    bt must be passed in)
    '''

    if type(target) is list:
        target = str(target[0])
    return exec_cmds(env, cmds, dir, silent, target)

#==============================================================================
# Exec CMDS
# Executes/Runs comdas passed in from builders, and respects the verbose
# levels. it returns a tuple (stdout, stderr, rv).
#
# Note: because of changes to capture command output on the fly, the stderr
# will always be an empty string stdout and stderr are now mixed in stdout.
#==============================================================================
def exec_cmds(env, cmds, dir=None, silent=None, target=None, \
    shell=False, buffer=True):
    '''
    exec_cmds

    Executes/Runs comdas passed in from builders, and respects the verbose
    levels. it returns a tuple (stdout, stderr, rv).

    Note: because of changes to capture command output on the fly, the stderr
    will always be an empty string stdout and stderr are now mixed in stdout.
    '''
    # call cmds
    #silent = GetOption('silent')
    if type(target) is list:
        target = str(target[0])

    verbose = int(GetOption('verbose'))
    all_data = ''
    all_err = ''
    all_rv = 0
    org_cwd = None
    cmd_env = None
    use_shell = shell
    stdout_data = ''

    # clean command environment,
    # it must contain only strings otherwise popen will fail
    if env.has_key('ENV'):
        org_env = env['ENV']
        cmd_env = {}
        for key in org_env.keys():
            cmd_env[key] = str(org_env[key])

    if verbose > 1:
        if target == None:
            target_str = ""
        else:
            target_str = ", Creating [" + target + "]"

    # calculate steps,
    # and make sure cmds is a list other wise make it a list
    current_step = 1
    if type(cmds) is not list:
        cmds = [cmds]

    total_steps = str(len(cmds))

    # if dir is not None, change directory to dir
    if dir is not None:
        dir = env.NormPath(dir)

        if verbose == 3:
            print "cd " + dir
        elif verbose == 2:
            print "== Change Directory: " + dir + " =="

        org_cwd = os.getcwd()
    else:
        dir = os.getcwd()

    # go trough list of commands !!cmds must be a list and not an single string
    for cmd in cmds:
        cmd = env.subst(cmd)

        if verbose == 3:
            print cmd
        elif verbose == 2:
            print '<== Step '+ str(current_step) +' of '+ total_steps + \
                target_str + ' \nCommand:\n'+ cmd +'\n==>'

        startupinfo = None
        if os.name == 'nt':
            startupinfo = subprocess.STARTUPINFO()
        else:
            use_shell = True

        # check for shell operations
        if re.search(r"(<|>|&&|\|)", cmd):
            use_shell = True

        try:
            #pathdirs = os.environ["PATH"]
            proc = subprocess.Popen(
                cmd,
                #stdin=PIPE,                # don't take imput away from tools
                stdout=subprocess.PIPE,
                #stderr=subprocess.PIPE,
                stderr=subprocess.STDOUT,   # need std<out & err> combined
                startupinfo=startupinfo,
                shell=use_shell,
                cwd=dir
                #env=cmd_env
                #env={"PATH":pathdirs}
            )

            stdout_line = proc.stdout.readline()

            if buffer:
                stdout_data = stdout_line

            while stdout_line:
                # Print the line out (remove trailing whitespace)
                if not silent:
                    print stdout_line.rstrip()

                # Read the next line
                stdout_line = proc.stdout.readline()

                if buffer:
                    stdout_data = "".join([stdout_data, stdout_line])

            # Wait for the process to exit and get exit code
            rcv = proc.wait()
            #import pdb;pdb.set_trace()

        except:
            env.PrintError("------------------------------------------------" \
                "-------------------------------")
            env.PrintError("*** Popen exception.")
            env.PrintError("*** CMD:%s" % cmd)
            env.PrintError("*** OS Name=%s" % os.name)
            env.PrintError("*** startupinfo = %s" % str(startupinfo))
            env.PrintError("*** sys exec info:")
            env.PrintError("%s" % str(sys.exc_info()))
            env.PrintError("------------------------------------------------" \
                "-------------------------------")
            raise RuntimeError('Exception from subprocess.Popen')

        current_step += 1

        if len(stdout_data) > 0:
            all_data = "".join([all_data, stdout_data])

    if org_cwd is not None:
        if verbose == 3:
            print "cd " + org_cwd
        elif verbose == 2:
            print "== Change Directory: " + org_cwd + " =="

    return [all_data, all_err, rcv]

#==============================================================================
# load_scons_scripts
#
#==============================================================================
def load_scons_scripts(env, scripts):
    '''
    load_scons_scripts
    '''
    # init input vars
    if type(scripts) is not list:
        scripts = [scripts]

    for script in scripts:
        script = env.subst(script)
        env.PrintDebugInfo \
            ('scripts', "Loading script: " + env.RealPath(script))
        env.SConscript(script, exports='env')

    return None

#==============================================================================
# load_tool_script
#
#==============================================================================
def load_tool_script(env, tool, toolpath=[]):
    '''
    load_tool_script
    '''
    (tool_base, tool_ext) = os.path.splitext(tool)

    if env.PathExists(tool) and tool_ext.lower() == ".py":
        (toolpath, tool) = os.path.split(tool_base)
        toolpath = [env.NormPath(toolpath)]
    else:
        if type(toolpath) is not list:
            toolpath = [toolpath]

        toolpath = [env.NormPath(dir) for dir in toolpath]

    m_fp, m_path, m_desc = imp.find_module(tool, toolpath)

    if m_fp is None:
        env.PrintWarning("Tool ["+ tool +"] not found in "+ str(toolpath))
    else:
        env.PrintDebugInfo \
            ('scripts', "Loading tool script: " + env.RealPath(m_path))
        try:
            m = imp.load_module(tool, m_fp, m_path, m_desc)
        finally:
            # Since we may exit via an exception, close fp explicitly.
            if m_fp:
                m_fp.close()

        # module loaded, now generate
        if env.get('LOADED_BUILDER'):
            env.AddLoadedBuilder(env.RealPath(m_path))
        m.exists(env)
        m.generate(env)

    return None

#==============================================================================
# load_image_units
#
#==============================================================================\
def load_image_units(env, dir_patterns=None):
    '''
    load_image_units
    '''
    #import pdb; pdb.set_trace()
    if dir_patterns is None:
        dir_patterns = env.get('IMAGE_DIR_PATTERNS')
    if type(dir_patterns) is not list:
        dir_patterns = [dir_patterns]

    script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')

    load_sub_scripts(env, dir_patterns, script_name_patterns, "image", clone_env=True)

    return None

#==============================================================================
# load_api_units
#
#==============================================================================
def load_api_units(env, dir_patterns=None):
    '''
    load_api_units
    '''
    #import pdb; pdb.set_trace()
    if dir_patterns is None:
        dir_patterns = env.get('SCRIPT_DIR_PATTERNS')

    if type(dir_patterns) is not list:
        dir_patterns = [dir_patterns]

    script_name_patterns = env.get('API_SCRIPT_NAME_PATTERNS')

    load_sub_scripts(env, dir_patterns, script_name_patterns, "api")

    return None

#==============================================================================
# load_sw_units
# Level param is not used any longer it will be obsoleated eventually, for the
# mean time mantain backward compatibility
#==============================================================================
def load_sw_units(env, level=0, dir_patterns=None):
    '''
    load_sw_units

    Level param is not used any longer it will be obsoleated eventually, for the
    mean time mantain backward compatibility
    '''
    if dir_patterns is None:
        dir_patterns = env.get('SCRIPT_DIR_PATTERNS')

    if type(dir_patterns) is not list:
        dir_patterns = [dir_patterns]

    node = SoftwareUnitsNode(env)
    node.load_sw_units(dir_patterns)
    return None

#==============================================================================
# get_sw_units
# same bheaivor as load_sw_units, but will create the units buckets as AU does
# (load_au_sw_units) allowing for the user of this to collect the units. And do
# soemthing else before passing them up to the iamge.
#==============================================================================
def get_sw_units(env, dir_patterns=None):
    '''
    get_sw_units

    same bheaivor as load_sw_units, but will create the units buckets as AU does
    (load_au_sw_units) allowing for the user of this to collect the units.
    And do soemthing else before passing them up to the iamge.
    '''
    env = env.Clone()
    env.Replace(ROOT_ENV=env)
    env.Replace(AU_ENV=env)
    env.Replace(AU_LIBS=[])
    env.Replace(AU_OBJS=[])
    env.Replace(AU_DOCS=[])
    env.Replace(AU_LISTFILES=[])
    env.Replace(AU_ARTIFACTS=[])
    env.Replace(AU_SHARED_LIBS=[])
    env.Replace(AU_PIC_OBJS=[])

    load_sw_units(env, dir_patterns=dir_patterns)

    # after loading scripts get list of libs and objs
    #import pdb; pdb.set_trace()
    items = dict()
    items['LIBS'] = env.get('AU_LIBS')
    items['OBJS'] = env.get('AU_OBJS')
    items['DOCS'] = env.get('AU_DOCS')
    items['LISTFILES'] = env.get('AU_LISTFILES')
    items['ARTIFACTS'] = env.get('AU_ARTIFACTS')
    items['ENV'] = env.Clone()
    items['SHARED_LIBS'] = env.get('AU_SHARED_LIBS')
    items['PIC_OBJS'] = env.get('AU_PIC_OBJS')

    return items

#==============================================================================
# get_au_sw_unit_names
#
#==============================================================================
def get_au_sw_unit_names(env):
    '''
    get_au_sw_unit_names
    '''
    au_root_script_pattern = env.get('AU_ROOT_DIR_PATTERNS')
    script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')

    # compine dir and file patterns
    patterns = ["".join([directory, "/", file_item]) \
        for directory in au_root_script_pattern \
        for file_item in script_name_patterns]

    # glob files matching patterns
    au_root_scripts = env.GlobFiles(patterns, posix=True)

    #import pdb; pdb.set_trace()
    # if BUILD_ROOT is not the base pattern for all, this will not work
    # and a multiple matches must be done one for each pattern, most probably
    # replace '*' with '(\w+)' to each au_root_dir_pattern to come up with
    # the matching reg-ex
    au_current_list = []
    au_name_re = re.compile(r'^' + env.subst('${BUILD_ROOT}/') + '(\w+)/')

    for au_root_script in au_root_scripts:
        re_match = au_name_re.match(au_root_script)
        if re_match:
            au_current_list.append(re_match.group(1))

    # update global enviroment so other images can use this information
    # and not have to re-calculate
    root_glocal_env = env.get('ROOT_GLOBAL_ENV')
    if root_glocal_env is not None:
        root_glocal_env.Replace(AU_CURRENT_LIST=au_current_list)

    return au_current_list

#==============================================================================
# load_au_sw_units
#
# Returns dictionary with data accessed by keys:
#    LIBS, libraries nodes
#    OBJS, objects files nodes
#    DOCS, document files nodes
#    ENV,  AU enviroment.
#
#==============================================================================
def load_au_sw_units(env, au_name, filter_list=[]):
    '''
    load_au_sw_units

    Returns dictionary with data accessed by keys:
      LIBS, libraries nodes
      OBJS, objects files nodes
      DOCS, document files nodes
      ENV,  AU enviroment.
    '''
    #import pdb; pdb.set_trace()
    mark_time_start(env, "Analysis of Component %s" % au_name)

    #env.PrintDebugInfo('time', "Loading %s scripts..." % au_name)
    env.PrintInfo("Loading %s scripts..." % str(au_name).upper())

    # init parameter vars
    if type(filter_list) is not list:
        filter_list = [filter_list]

    # get current au list (or build it) and check for valid AU
    au_current_list = env.get('AU_CURRENT_LIST')
    script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')

    if au_current_list is None:
        au_current_list = env.GetAreaSoftwareUnitNames()

    if au_name not in au_current_list:
        env.PrintError("[" + au_name +"] is not a valid AU name.")
        raise RuntimeError('Invalid AU given')

    # init enviroment variables
    root_env = env.get('ROOT_ENV')
    env = env.Clone()

    if root_env is not None:
        root_env["au_" + au_name + '_env'] = env

    # Find a better configurable way of adding this buckets, with
    #addition of SAHRED_LIBS and coming soon LTO_LIBS, this has a potential for
    # being a maintenance disaster There always seems to be a need for buckets
    # at a AU and image levels; find a better way.
    env.Replace(ROOT_ENV=env)
    env.Replace(AU_ENV=env)
    env.Replace(AU_NAME=au_name)
    env.Replace(AU_LIBS=[])
    env.Replace(AU_OBJS=[])
    env.Replace(AU_DOCS=[])
    env.Replace(AU_LISTFILES=[])
    env.Replace(AU_ARTIFACTS=[])
    env.Replace(AU_SHARED_LIBS=[])
    env.Replace(AU_PIC_OBJS=[])

    # init restricted inc paths
    env.InitAUIncPaths()

    #debug filter
    #filter_list=['debugtools/tmc', 'services', 'storage/efs']

    # init this AU filter list and default to load au
    au_filter_list = []
    do_load_au = True

    # get command line filter options
    cmd_filter_opt = env.get('FILTER_OPT')
    #import pdb; pdb.set_trace()
    if cmd_filter_opt is not None:
        filter_opt_list = cmd_filter_opt

        # if there are both command line filters and AU filters
        # these need to be merged.
        # the rule is AU filters are the only ones we can use as
        # a base to come up with final filter list.
        #  examples:
        #  cmd filter     AU filter      Final Filter
        #  X/Y/Z          X/Y            X/Y/Z
        #  X              X/Y            X/Y
        #  A/B/C          X/Y            nothing to build
        #  A/B/C          X/Y,A          A/B/C
        #  A/B/C          X/Y,A/D        nothing to build
        if len(filter_list) and do_load_au:
            for au_filter in filter_list:
                au_filter = "".join \
                    ([au_name, env.get('FILTER_SEP', '/'), au_filter])
                for filter_opt in filter_opt_list:
                    if len(filter_opt) >= len(au_filter):
                        if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
                            if string.lower(au_filter) == \
                                string.lower(filter_opt[:len(au_filter)]):
                                au_filter_list.append(filter_opt)
                        else:
                            if au_filter == filter_opt[:len(au_filter)]:
                                au_filter_list.append(filter_opt)
                    else:
                        if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
                            if string.lower(filter_opt) == \
                                string.lower(au_filter[:len(filter_opt)]):
                                au_filter_list.append(au_filter)
                        else:
                            if filter_opt == au_filter[:len(filter_opt)]:
                                au_filter_list.append(au_filter)

        # no AU filters, process only cmd line filters
        else:
            au_filter_list = filter_opt_list

        # use only this au filters.
        if len(au_filter_list):
            au_filters = \
                env.PathToDictTree(au_filter_list, env.get('FILTER_SEP', '/'))
            au_filters = au_filters.get(au_name)

            if au_filters is not None:
                env.Replace(AU_FILTERS=au_filters)
            else:
                do_load_au = False

        # filter nuke each other out nothing to build
        else:
            do_load_au = False

    # there are no command line filter options
    else:
        if len(filter_list):
            au_filters = \
                env.PathToDictTree(filter_list, env.get('FILTER_SEP', '/'))
            env.Replace(AU_FILTERS=au_filters)

    # time to load root script
    if do_load_au:
        #import pdb; pdb.set_trace()
        # get au root script
        au_dir_patterns = env.get('AU_DIR_PATTERNS')
        patterns = ["".join([dir, "/", file]) \
            for dir in au_dir_patterns for file in script_name_patterns]
        au_root_script = env.GlobFiles(patterns)[0]

        env.PrintDebugInfo('scripts', "Loading " + \
            au_name.upper() + " scripts... ")
        env.PrintDebugInfo('scripts', "Loading script: " + \
            env.RealPath(au_root_script))
        env.SConscript(au_root_script, exports='env')

    # after loading scripts get list of libs and objs
    #import pdb; pdb.set_trace()
    items = dict()
    items['LIBS'] = env.get('AU_LIBS')
    items['OBJS'] = env.get('AU_OBJS')
    items['DOCS'] = env.get('AU_DOCS')
    items['LISTFILES'] = env.get('AU_LISTFILES')
    items['ARTIFACTS'] = env.get('AU_ARTIFACTS')
    items['ENV'] = env.Clone()
    items['SHARED_LIBS'] = env.get('AU_SHARED_LIBS')
    items['PIC_OBJS'] = env.get('AU_PIC_OBJS')

    # add items to IMAGE lists
    image_env = env.get('IMAGE_ENV')

    if image_env is not None:
        image_libs = image_env.get('IMAGE_LIBS')
        if image_libs is not None:
            image_libs.extend(env.get('AU_LIBS'))

        image_objs = image_env.get('IMAGE_OBJS')
        if image_libs is not None:
            image_objs.extend(env.get('AU_OBJS'))

        image_docs = image_env.get('IMAGE_DOCS')
        if image_libs is not None:
            image_docs.extend(env.get('AU_DOCS'))

        image_listfiles = image_env.get('IMAGE_LISTFILES')
        if image_libs is not None:
            image_listfiles.extend(env.get('AU_LISTFILES'))

        image_artifacts = image_env.get('IMAGE_ARTIFACTS')
        if image_libs is not None:
            image_artifacts.extend(env.get('AU_ARTIFACTS'))

    mark_time_end(env, "Analysis of Component %s" % au_name)
    #print "** Load AU Elapsed time = " + str(round(t_end - t_start, 3))
    #env.PrintDebugInfo('time', "Load %s scripts, elapsed time %s" % \
    #   (au_name, str(round(t_end - t_start, 3))))
    #env.PrintInfo("Loaded %s scripts, elapsed time %s sec." % \
    #    (str(au_name).upper(), str(round(t_end - t_start, 3))))

    return items

#==============================================================================
# load_sub_units
# This fucntion is intended to load non library scripts such as image and
# API definition scripts
#==============================================================================
def load_sub_scripts(env, script_dir_patterns, script_name_patterns, \
    script_type, clone_env=False):
    '''
    load_sub_scripts

    This fucntion is intended to load non library scripts such as image and
    API definition scripts
    '''
    #import pdb; pdb.set_trace()
    node = SoftwareUnitsNode(env)
    script_list = node.get_files(script_dir_patterns, script_name_patterns)
    script_type = "".join([script_type, " "])
    node.load_scripts(script_list, script_type, clone_env=clone_env, mark_time=True)
    return None

#==============================================================================
# SoftwareUnitsNode
#   this class deal with loading the correct sripts at any script node that
#   has sub nodes, since the scripts need to be read and they may have
#   different rules, more nodes needs to be created from with in the SCons
#   scripts.
#==============================================================================
class SoftwareUnitsNode:
    '''
    class SoftwareUnitsNode

    This class deal with loading the correct sripts at any script node that
    has sub nodes, since the scripts need to be read and they may have
    different rules, more nodes needs to be created from with in the SCons
    scripts.
    '''
    def __init__(self, env):
        '''
        __init__
        '''
        self.verbose = int(GetOption('verbose'))
        self.debuginfo_opt = env.get('DEBUGINFO_OPT')
        self.env = env

    def get_files(self, dir_patterns, file_patterns, posix=False):
        '''
        get_files
        '''
        # init vars
        env = self.env

        if type(dir_patterns) is not list:
            dir_patterns = [dir_patterns]

        if type(file_patterns) is not list:
            file_patterns = [file_patterns]

        # combine dir and file patterns
        patterns = ["".join([dir, "/", file]) \
            for dir in dir_patterns for file in file_patterns]

        # glob files from dir/file patterns
        file_list = env.GlobFiles(patterns, posix)

        return file_list

    def load_scripts(self, scripts, script_type="", clone_env=False, mark_time=False):
        '''
        load_scripts
        '''
        # init vars
        env = self.env

        if type(scripts) is list:
            for script in scripts:
                org_env = env
                if clone_env:
                    env = env.Clone()

                relpath = env.RelPath(script, "$TARGET_ROOT")

                env.PrintDebugInfo('scripts', "Loading "+ script_type + \
                    "script: " + env.RealPath(script))
                if mark_time and 'image' in script_type:
                    mark_time_start(env, "Analysis of Image %s" % (relpath))
                env.SConscript(script, exports='env')
                if mark_time and 'image' in script_type:
                    mark_time_end(env, "Analysis of Image %s" % (relpath))
                env = org_env

        else:
            org_env = env
            if clone_env:
                env = env.Clone()
            relpath = env.RelPath(scripts, "$TARGET_ROOT")
            env.PrintDebugInfo('scripts', "Loading "+ script_type + \
                "script: " + env.RealPath(scripts))
            if mark_time and 'image' in script_type:
                mark_time_start(env, "Analysis of Image %s" % (relpath))
            env.SConscript(scripts, exports='env')
            if mark_time and 'image' in script_type:
                mark_time_end(env, "Analysis of Image %s" % (relpath))

            env = org_env

        return None

    def load_sw_units(self, dir_patterns):
        '''
        load_sw_units
        '''
        #import pdb; pdb.set_trace()

        # init filters and list of script vars
        env = self.env
        filters = env.get('AU_FILTERS')
        script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')
        filter_script_list = []

        # find all matching scripts at this level
        script_list = self.get_files(dir_patterns, script_name_patterns)

        # if no matching scripts where fund return
        if len(script_list) == 0:
            if 'scripts' in self.debuginfo_opt:
                env.PrintWarning("No scripts found from [" + \
                    env.RealPath('.') + "],\n" + "   with dir pattern " + \
                    str(dir_patterns) + ".")

            return None

        # clean list depending on filters, and add filters to list
        # this filters will get pass down to sub nodes with the
        # AU_FILTERS env var. The trick is that each sub node gets
        # only the part of the filter it cares about and not all of it.
        if filters is not None and len(filters.keys()) > 0:
            for key in filters.keys():
                filter_match = False
                dir_viable_list = \
                    [dir.replace('*', key) for dir in dir_patterns]
                for dir in dir_viable_list:
                    for script in script_list:
                        dir_path = env.NormPath(dir, posix=True)
                        script_path = env.NormPath(script, posix=True)

                        # check for windows and lower case
                        if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
                            dir_path = string.lower(dir_path)
                            script_path = string.lower(script_path)

                        if dir_path == script_path[:len(dir_path)]:
                            filter_script_list.append([script, filters[key]])
                            filter_match = True

                if not filter_match:
                    #import pdb; pdb.set_trace()
                    env.PrintWarning("No scripts found mathicng filter [" + \
                        key +"],\n" + "   from [" + env.RealPath('.') + \
                        "],\n" + "   with dir pattern " + \
                        str(dir_patterns) + ".")

        # no filters, load all scripts found
        else:
            filter_script_list = [(script, None) for script in script_list]

        #import pdb; pdb.set_trace()

        for script_filter in filter_script_list:
            script = script_filter[0]
            filter = script_filter[1]

            # if there are filters set variable accordingly
            if filter is not None:
                env.Replace(AU_FILTERS=filter)

            self.load_scripts(script, clone_env=True, mark_time=False)
            #env.SConscript(script, exports='env')

#==============================================================================
# load_au_sw_units_deprecated /* DEPRECATED */
#
# Retunds two tuplet (libs, objs)
#   libs, libs nodes
#   objs, object files nodes
#
#==============================================================================
def load_au_sw_units_deprecated(env, au_name, filter_list=[]):
    '''
    load_au_sw_units_deprecated /* DEPRECATED */

    Retunds two tuplet (libs, objs)
     libs, libs nodes
     objs, object files nodes
    '''
    items = env.LoadAreaSoftwareUnits(au_name, filter_list)
    return items.get('LIBS'), items.get('OBJS')

#==============================================================================
# load Core BSP Lib Scripts /* DEPRECATED */
#==============================================================================
def load_cbsp_lib_scripts_deprecated(env):
    '''
    load_cbsp_lib_scripts_deprecated
    '''

    # deprecated don't use

    items = env.LoadAreaSoftwareUnits("core")

    #import pdb; pdb.set_trace()
    root_env = env.get('ROOT_ENV')
    libs = root_env.get('LIBS')
    libs += items.get('OBJS')
    libs += items.get('LIBS')

#==============================================================================
# load external Lib Scripts /* DEPRECATED */
#==============================================================================
def load_ext_lib_scripts_deprecated(env):
    '''
    load_ext_lib_scripts_deprecated
    '''
    # deprecated don't use
    return None

#==============================================================================
# MD5 sum for files
# returns hex digist
#==============================================================================
def md5file(env, file_names):
    '''
    md5file

    MD5 sum for files
    returns hex digist
    '''
    md5data = hashlib.md5()

    # if file_names is not a list make it a list
    if type(file_names) != list:
        file_names = [file_names]

    for filename in file_names:
        file = open(filename)
        while 1:
            data = file.read(4096)
            if data == "":
                break
            md5data.update(data)
        file.close()
    return md5data.hexdigest()

#==============================================================================
# MD5 sum create md5 data file
# MD5 sums file and creates a md5 file
#==============================================================================
def md5file_create(env, file_names, md5_file_name):
    '''
    md5file_create

    MD5 sum create md5 data file
    MD5 sums file and creates a md5 file
    '''
    md5sum_data = md5file(env, file_names)

    md5_file_name = env.NormPath(md5_file_name)

    md5_file = open(md5_file_name, "w")
    md5_file.write(md5sum_data)
    md5_file.close()
    return md5sum_data

#==============================================================================
# MD5 sum compare and update md5 data file
#
#==============================================================================
def md5file_compare(env, file_name, md5_file_name):
    '''
    md5file_compare

    MD5 sum compare and update md5 data file
    '''
    md5sum_data = 0

    md5_file_name = env.NormPath(md5_file_name)

    if os.path.exists(md5_file_name):
        md5_file = open(md5_file_name, "r")
        md5sum_data = md5_file.read(4096)
        md5_file.close()

    md5sum_new_data = md5file(env, file_name)

    if md5sum_data == md5sum_new_data:
        result = True
    else:
        result = False

    return result

#==============================================================================
# MD5 sum compare and update md5 data file
#
#==============================================================================
def md5file_compare_and_update(env, file_name, md5_file_name):
    '''
    md5file_compare_and_update

    MD5 sum compare and update md5 data file
    '''
    result = env.CompareMD5File(file_name, md5_file_name)

    if not result:
        env.CreateMD5SumFile(file_name, md5_file_name)

    return result

#==============================================================================
# Remove Files
#
#==============================================================================
def remove_file(env, file_names):
    '''
    remove_file

    Remove Files
    '''
    # if file_names is not a list make it a list
    if type(file_names) != list:
        file_names = [file_names]

    for filename in file_names:
        fname = env.NormPath(filename)
        if os.path.exists(fname):
            os.unlink(fname)
            print "Removed " + fname

    return None

#==============================================================================
# Find Files
#
#==============================================================================
def find_files(env, file_name_patterns='*', root_dir='.', \
    posix=False, sort=True):
    '''
    find_files

    Find Files
    '''
    # initialize variables
    ospath_join = os.path.join
    env_subst = env.subst
    aggregate_file_list = []

    # expand any SCons variables
    root_dir = env_subst(root_dir)

    # make sure patterns is a list
    if type(file_name_patterns) is not list:
        file_name_patterns = [file_name_patterns]

    # walk trough each directory and try each pattern, aggregate results.
    for dirpath, dirs, files in os.walk(root_dir):

        for file_name_pattern in file_name_patterns:

            file_name_pattern = env_subst(file_name_pattern)

            files_match = fnmatch.filter(files, file_name_pattern)

            match_file_list = \
                [ospath_join(dirpath, file_item) for file_item in files_match]
            match_file_list = env.RealPath(match_file_list)
            aggregate_file_list.extend(match_file_list)

    # remove any duplicate matches from aggregate list.
    aggregate_file_list = list(set(aggregate_file_list))

    if posix:
        aggregate_file_list = \
            [file_item.replace('\\', '/') for file_item in aggregate_file_list]

    if sort:
        aggregate_file_list = sorted(aggregate_file_list)

    return aggregate_file_list

#==============================================================================
# Find Source Files
#
#==============================================================================
def find_source_files(env, file_name_patterns, srcpath, \
    posix=False, sort=True):
    '''
    find_source_files

    Find Source Files
    '''
    real_srcpath = env.RealPath(srcpath, posix)

    file_list = find_files(env, file_name_patterns, real_srcpath, posix, sort)
    file_list = [file_item.replace(real_srcpath, "${BUILDPATH}") \
        for file_item in file_list]
    #import pdb; pdb.set_trace()

    return file_list


#==============================================================================
# Glob Files
#
#==============================================================================
def glob_files(env, glob_patterns=["*"], posix=False, sort=True):
    '''
    glob_files

    Glob Files
    '''

    if env.get('PLATFORM', 'none') == 'cygwin':
        posix = True

    #import pdb; pdb.set_trace()
    # initialize variables
    env_subst = env.subst
    glob_glob = glob.glob
    glob_list = []
    glob_list_extend = glob_list.extend

    # make sure patterns is a list
    if type(glob_patterns) is not list:
        glob_patterns = [glob_patterns]

    glob_patterns = [env_subst(pattern) for pattern in glob_patterns]
    [glob_list_extend(glob_glob(pattern)) for pattern in glob_patterns]

    # remove any duplicate matches from aggregate list.
    glob_list = list(set(glob_list))

    if posix:
        glob_list = [glob_item.replace('\\', '/') for glob_item in glob_list]

    if sort:
        glob_list = sorted(glob_list)

    return glob_list

#==============================================================================
# Glob Source Files
#
#==============================================================================
def glob_source_files(env, glob_patterns, srcpath, posix=False, sort=True):
    '''
    glob_source_files

    Glob Source Files
    '''
    def norm_path(path, posix):
        '''
        norm_path
        '''
        npath = os.path.normpath(path)
        if posix:
            npath = npath.replace('\\', '/')
        return npath

    if type(glob_patterns) is not list:
        glob_patterns = [glob_patterns]

    real_srcpath = env.RealPath(srcpath, posix)
    real_glob_patterns = \
        ["".join([real_srcpath, "/", pattern]) for pattern in glob_patterns]

    file_list = glob_files(env, real_glob_patterns, posix, sort)
    file_list = [norm_path(file_item.replace(real_srcpath, "${BUILDPATH}"), \
        posix) for file_item in file_list]
    #import pdb; pdb.set_trace()

    return file_list


#==============================================================================
# add_explicit_deps
# adds explicit dependencies to the enviroment, the different compiler tools
# scripts will add this explicit dependencies to each .c or .cpp
#==============================================================================
def add_explicit_deps(env, files):
    '''
    add_explicit_deps

    adds explicit dependencies to the enviroment, the different compiler tools
    scripts will add this explicit dependencies to each .c or .cpp
    '''
    env.Append(EXPLICIT_DEPS=[files])


#==============================================================================
# find_env_var_by_value
# Find all SCons Build Environment variables that contain the specified value
#==============================================================================
def find_env_var_by_value(env, values):
    '''
    find_env_var_by_value

    Find all SCons Build Environment variables that contain the specified value
    '''
    if type(values) != list:
        values = [values]

    for key, env_val in env.items():
        if type(env_val) == str and \
            True in [fnmatch.fnmatch(env_val, x) for x in values]:
            env.PrintInfo("env['" + str(key) + "'] = " + env_val)

#==============================================================================
# store_linking_start_time
#
# stores start time of linking image.
# If this method is called more than once for the same target, then print a
# warning and do not store the link start time
#==============================================================================
def store_linking_start_time(*nargs, **kwargs):
    try:
        if not kwargs.has_key('env'):
            #env is not available, so cannot print any warnings
            return

        env = kwargs['env']
        if not kwargs.has_key('target'):
            env.PrintWarning("StoreLinkingStartTime can only be called with SCons's AddPreAction method and target should not be None")
            return

        if not env.has_key('IMAGE_LINK_START_TIME'):
            env.Replace(IMAGE_LINK_START_TIME={})

        #Extra check to see if the target is a list
        if type(kwargs['target']) is not list:
            return

        #Check if this function was icalled for the same target , if so, that is a wrong usage, delete the entry from dictionary and print a warning message
        target = (kwargs['target'])[0]
        image_start_time_dict = env.get('IMAGE_LINK_START_TIME')
        if image_start_time_dict and image_start_time_dict.has_key(target):
            del image_start_time_dict[target]
            env.PrintWarning("StoreLinkingStartTime should not be called multiple times for the same target " + target)
        else:
            image_start_time_dict[target] = time.time()
            env.PrintDebugInfo("linker start Time: " + str(image_start_time_dict[target]), None, None)
    except:
        pass

#==============================================================================
# print_overall_link_time
#
# prints overall link time for a given image.
# If this method is called before calling StoreLinkingStartTime for the same
# target, then print a  warning and do not print the overall link time
#==============================================================================
def print_overall_link_time(*nargs, **kwargs):
    try:
        if not kwargs.has_key('env'):
            #env is not available, so cannot print any warnings
            return


        if not kwargs.has_key('target'):
            env.PrintWarning("PrintOverallLinkTime can only be called with SCons's AddPostAction method and target should not be None")
            return

        env = kwargs['env']
        #Extra check to see if the target is a list
        if type(kwargs['target']) is not list:
            return

        target = (kwargs['target'])[0]

        if not env.has_key('IMAGE_LINK_START_TIME') and not env['IMAGE_LINK_START_TIME']:
            env.PrintWarning("StoreLinkingStartTime should be called first for target" + target.rstr() + " for getting the overall link time")
            return


        #Check if this function was called for the same target , if so, that is a wrong usage, delete the entry from dictionary and print a warning message
        image_start_time_dict = env.get('IMAGE_LINK_START_TIME')
        if (image_start_time_dict is not None) and (not image_start_time_dict.has_key(target)):
            env.PrintWarning("StoreLinkingStartTime should be called first for target" + target.rstr() + " for getting the overall link time")
        else:
            overall_link_time = round((time.time()-image_start_time_dict[target]), 2)
            env.PrintInfo("Link time for " + target.rstr() + ": "  + str(overall_link_time)+ " seconds")
    except:
        pass

#===============================================================================
# write_image_info
#
# Write the specified info to the image_info.json file.
#===============================================================================
def write_image_info (env, name, info):
    """
    Write the specified info into the image_info.json file.

    Parameters:

        name:  A short descriptive name for the info.

        info:  The info to be written.  This info should either be a string,
               integer, or dictionary that can be turned into json data.
    """

    if env.IsBuildInternal():

        acquire_mp_mutex(env, 'image_info')

        info_str = json.dumps(info)
        cmd = ['image_info_writer',
               '--target', env.subst('${BUILD_ROOT}'), '--verbose',
               '--variant', env.subst('${BUILD_ID}'),
               '--add', '--info_name', name, '--info', info_str]
        subprocess.call(cmd)

        release_mp_mutex(env, 'image_info')
    
# End of write_image_info()

#===============================================================================
# acquire_mp_mutex
#
# Acquire the specified mutex.  If no such mutex exists, create one.
#===============================================================================
mutex_dir = os.path.join(os.path.dirname(__file__), 'mp_mutexes')
mutex_timers = {}
mutex_wait_time = 1    # Seconds
mutex_max_retries = 10   # Used only for corner cases to stop looping forever.
                         # Not used in normal case of when someone has mutex reserved.

def acquire_mp_mutex (env, mutex_name):

    mutex_file_name = os.path.join(mutex_dir, mutex_name + ".mpmutex")
    if not os.path.isdir(mutex_dir):
        try:
            # Put this in a try/except just in case another process creates
            # this directory out from under us.
            os.mkdir(mutex_dir)
        except OSError, e:
            if e.errno != errno.EEXIST:
                raise
    pid = os.getpid()
    acquired = False
    retry_count = 0

    while not acquired:
        try:
            # Open for create.  This should be an automic operation that will
            # fail if the file already exists.
            fd = os.open(mutex_file_name, os.O_EXCL | os.O_RDWR | os.O_CREAT)
            acquired = True
            # Now that we have the mutex, write our pid and return
            mutex_data = {'pid':pid,'aquire_date_time':time.asctime()}
            os.write(fd, json.dumps(mutex_data))
            os.close(fd)

        except OSError:
            try:
                fd = os.open(mutex_file_name, os.O_RDWR)
                mutex_data_str = os.read(fd, 100)
                os.close(fd)
                mutex_data = json.loads(mutex_data_str)
                if mutex_data['pid'] == pid:
                    # Handle the case where this process has already acquired the
                    # mutex
                    acquired = True
                elif not is_valid_pid(mutex_data['pid']):
                    # Whoever created this file is long gone, so let's delete it
                    # and try again
                    os.remove(mutex_file_name)
                else:
                    # We don't have the mutex, so sleep awhile and try again.
                    time.sleep(mutex_wait_time)
            except ValueError:
                retry_count += 1
                if retry_count > mutex_max_retries:
                    raise
                # Someone has allocated the mutex (created the file), but hasn't
                # yet written data to the file.  Just loop and try again in 1 second.
                time.sleep(mutex_wait_time)
            except OSError:
                # The file may have been deleted between when we checked for it
                # and when we tried to read it.  Loop immediately and retry.
                retry_count += 1
                if retry_count > mutex_max_retries:
                    raise
    # End of while not acquired

    # Print a warning if the mutex hasn't been freed in 10 seconds.

    if 'SCONS_MUTEX_TIMEOUT_PERIOD' not in env:
        env['SCONS_MUTEX_TIMEOUT_PERIOD'] = 10  # Default to 10 seconds to
                                                # wait for mutex to be freed.
    mutex_timers[mutex_name] = threading.Timer(env['SCONS_MUTEX_TIMEOUT_PERIOD'],
                                               print_warning,
                                               (env, "A mutex named '" + mutex_name +
                                                "' has not been freed."))
    mutex_timers[mutex_name].start()

# End of acquire_mp_mutex()

#===============================================================================
# release_mp_mutex
#
# Release the specified mutex.  If no such mutex exists, create one.
#===============================================================================
def release_mp_mutex (env, mutex_name):

    mutex_file_name = os.path.join(mutex_dir, mutex_name + ".mpmutex")
    pid = os.getpid()
    removed = False

    if os.path.exists(mutex_file_name):
        fd = os.open(mutex_file_name, os.O_RDWR)
        mutex_data_str = os.read(fd, 100)
        os.close(fd)
        mutex_data = json.loads(mutex_data_str)
        if mutex_data['pid'] == pid:
            # It's our file, so we can delete it.
            os.remove(mutex_file_name)
            mutex_timers[mutex_name].cancel()
            del mutex_timers[mutex_name]
            removed = True

    if not removed:
        print_warning(env, "Failed to remove mp mutex " + mutex_name +
                      ".  Indicated mutex does not exist, or is owned by"
                      " another process")

# End of release_mp_mutex()

#===============================================================================
# is_valid_pid
#
# Returns True if the given pid is valid
#===============================================================================

def is_valid_pid(pid):

    if os.name is 'posix':
        return os.path.isdir('/proc/' + str(pid))

    else:
        import ctypes
        kernel32 = ctypes.windll.kernel32
        SYNCHRONIZE = 0x100000

        process = kernel32.OpenProcess(SYNCHRONIZE, 0, pid)
        if process != 0:
            kernel32.CloseHandle(process)
            return True
        else:
            return False

# End of is_valid_pid()

#==============================================================================
# mark_start_time and mark_end_time
# These functions will mark the start and end time of a process and then log the delta
#==============================================================================
time_keeper = {}

def __delta_time(start, end):
    '''delta time'''
    delta = end - start
    minutes = delta / 60
    seconds = delta % 60
    delta_str = "%d minutes, %d seconds" % (minutes, seconds)
    return delta_str

def mark_time_start (env, description):
    '''mark time start'''
    if not description in time_keeper:
        time_keeper[description] = {}
    time_keeper[description]['StartTime'] = datetime.now()
    #env.PrintInfo('Starting: ' + description + '')

def mark_time_end (env, description):
    '''mark time end'''
    if not description in time_keeper:
        env.PrintWarning("Mark Time end called without a previous mark time start for description (%s)" % description)
        return
    time_keeper[description]['EndTime'] = datetime.now()
    env.PrintInfo('Elapsed time for ' + description + ' was [%s]' %  str((time_keeper[description]['EndTime'] - time_keeper[description]['StartTime'])))
