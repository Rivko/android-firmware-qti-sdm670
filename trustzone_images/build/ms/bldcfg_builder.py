
#===============================================================================
#
# SSG Build System
#
# Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
import os
import platform

try:
    import SCons.Script
except ImportError:
    pass # local execution

import build_all
import BuildConfigParser

#----------------------------------------------------------------------------
# Global values
# Global definitions used within this script for example tool cmd definitions
# which might be used in different places.
#----------------------------------------------------------------------------
BUILDER_NAME = "BuildConfigBuilder"

BUILD_CONFIG_INSTRUCTIONS = """IMPORTANT: To update any environment variable:
     - Find the {0} tag with the desired name under the desired OS
       (or common)
     - Change the 'method' property to "declare" (no 'method' implies "declare"
       automatically)
     - Set the 'value' property to the desired value
     - Explanations of each exported environment variable are found in that
       {0} tag's 'description' property
     - Example to change LLVMBIN (LLVM installation bin directory):
       Old:
        <var name="LLVMBIN"
                     method="declare"
                     value="/internal/tool/path"

       New:
        <var name="LLVMBIN"
                     value="/custom/llvm/directory/bin\"""".format( \
        "<" + BuildConfigParser.TAGS["variable"] + ">")

#----------------------------------------------------------------------------
# Hooks for SCons
# These are the function entry points SCons calls when loading this script.
#----------------------------------------------------------------------------
def exists(env):
    '''
    Used by SCons to make sure scripts are not allowed to be loaded multiple
    times per environment.
    '''
    return env.Detect('bldcfg_builder')

def generate(env):
    '''
    This is the entry point called by SCons when loading this script. It should
    call other generate functions since a script might define multiple builders
    or methods.
    '''
    bldcfg_generate(env)

#============================================================================
# build rules
#============================================================================
def bldcfg_generate(env):
    '''
    Generate function for build config builder, it will define the builder,
    methods to go with it, emitters, scanners, and build actions.
    '''
    # sometimes things need to be prepared or the client has to make a decision
    # whether this builder should be used depending on the image being loaded
    env.AddMethod(bldcfg_builder_wrapper, BUILDER_NAME)

    # Use the standard action wrapper provided by our SCons extensions. Provides
    # debug levels and correct text output during build. Not using this will
    # result in function pointers and mangled names being displayed instead of
    # nice strings such as "=== Generating  <your target>"
    bldcfg_act = env.GetBuilderAction(bldcfg_builder)


    bldcfg_bld = env.Builder(
        action = bldcfg_act,
        #emitter=bldcfg_emitter,
        #source_scanner=bldcfg_source_scanner,
        #target_scanner=bldcfg_target_scanner,
        #suffix = '.my_extension'
    )

    env.Append(BUILDERS = {'BuildConfigInternalBuilder' : bldcfg_bld})

#----------------------------------------------------------------------------
# builder wrapper
#----------------------------------------------------------------------------
def bldcfg_builder_wrapper(env, target, source):
    '''    
    This implementation is just a dummy wrapper function, but arguments of
    this function can be changed as needed whereas Builder arguments must
    always be exactly (target, source, env).
    '''
    return env.BuildConfigInternalBuilder(target, source)

#----------------------------------------------------------------------------
# builder action
#----------------------------------------------------------------------------
def bldcfg_builder(target, source, env):
    '''
    This is the actual builder action. It gets called only if the targets need
    to be built or rebuilt. Otherwise this will not be called. When done
    correctly (ie we tell scons our exact dependencies with scanners or
    emitters) this is a beautiful thing as it performs right action as needed.
    '''
    # Targets and sources are ALWAYS arrays (as per builder standard interface).
    # This builder needs to know if it deals with multiple targets and sources
    # or singles or a mix (common: one target, multiple sources).
    assert len(source) == 1, "{} requires exactly 1 source".format(BUILDER_NAME)
    assert len(target) == 1, "{} requires exactly 1 target".format(BUILDER_NAME)
    target_full = env.subst(str(target[0]))
    source_full = env.subst(str(source[0]))

    # validate environment values
    try:
        branch = SCons.Script.ARGUMENTS["SOFTWARE_IMAGE"]
    except NameError:  # local execution (not SCons)
        branch = env["SOFTWARE_IMAGE"]
        
    chipset = env["CHIPSET"]
    os_type = platform.system()

    # This builder just does everything here in Python and then uses open() &
    # <file>.write() generate the target.
    xml = BuildConfigParser.loadXml(source_full)
    minimized_xml = strip_xml(xml, branch, chipset, os_type)
    BuildConfigParser.dumpXml(minimized_xml, target_full)
    add_instructions(target_full)

#----------------------------------------------------------------------------
# builder emitter
#----------------------------------------------------------------------------
def bldcfg_emitter(target, source, env):
    '''
    Modify targets depending on sources (see SCons documentation) and also
    a good place to add Clean rules, Dependency rules, Side effects, etc.
    '''

    # Specifies a dependency to this builder such that if the given tool is
    # missing, the builder will fail. Also, if the tool changes (a version
    # update, for instance), the builder will be called to regenerate the
    # target even if the sources didn't change, since we are adding the tool as
    # a dependency.
    env.Depends(target, \
            env.subst("${BUILD_ROOT}/build/ms/BuildConfigParser.py"))

    return (target, source)

#----------------------------------------------------------------------------
# helper functions
#----------------------------------------------------------------------------
def strip_xml(xml_data, branch, chipset, os_type):
    '''
    Reduces the given loaded XML config to only include the current branch,
    chipset, & OS. Also removes intermediate variables & all aliases not marked
    as 'recompile'. Permanently modifies the input dictionary.
    '''
    # remove irrelevant branches
    assert branch in xml_data, \
            "build_config data does not contain branch '{}'".format(branch)
    for branch_name in xml_data.keys():
        if branch_name != branch:
            del xml_data[branch_name]

    # remove irrelevant chipsets
    chipset_data = xml_data[branch]["targets"]
    assert chipset in chipset_data, \
            "{} build_config data does not contain chipset '{}'".format( \
            branch, chipset)
    for chipset_name in chipset_data.keys():
        if chipset_name != chipset:
            del chipset_data[chipset_name]

    # remove irrelevant operating systems
    environment_data = chipset_data[chipset]["environment"]
    if os_type not in environment_data:
        print( \
                "OS type '{}' not in {} {} build_config data (defaulting to '{}')".format( \
                os_type, branch, chipset, build_all.DEFAULT_OS))
        os_type = build_all.DEFAULT_OS
        assert os_type in environment_data, \
                "BUG: default OS '{}' is invalid for {} on {}".format( \
                os_type, chipset, branch)
    for os_name in environment_data.keys():
        if os_name != os_type:
            del environment_data[os_name]

    # remove intermediate variables
    var_data = environment_data[os_type]["variables"]
    for var_name in var_data.keys():
        if not var_data[var_name].get("export"):
            del var_data[var_name]

    # remove aliases that do not have 'recompile' set
    alias_data = chipset_data[chipset]["files"]
    for alias in alias_data.keys():
        if not alias_data[alias]["recompile"]:
            del alias_data[alias]
            continue

        # external/recompile must never honor 'internal-test' (ie USES_NO_CP)
        # as it will cause build & functionality failures in HY11/HK11
        if alias_data[alias].has_key("internal-test"):
            alias_data[alias]["internal-test"] = False

        # remove any custom arguments
        if alias_data[alias].get("arguments"):
            alias_data[alias]["arguments"] = []

        # remove any custom parameters
        if alias_data[alias].get("params"):
            alias_data[alias]["params"] = []

    return xml_data

def add_instructions(xml_file):
    '''
    Add instructions to the top of the XML file as a comment.
    '''
    assert os.path.isfile(xml_file), "{} does not exist".format(xml_file)
    with open(xml_file) as infile:
        lines = infile.read().splitlines()

    assert len(lines) > 2, "{} is not a valid build config XML".format(xml_file)
    comment = [
        "<!-- ",
        BUILD_CONFIG_INSTRUCTIONS,
        " -->",
    ]

    new_lines = [ lines[0] ] + "".join(comment).splitlines() + lines[1:]

    with open(xml_file, "w") as outfile:
        outfile.write("\n".join(new_lines))

def _test():
    '''
    Command-line test of load & strip
    '''
    import sys

    class FakeEnv(dict):
        def subst(self, string):
            return string

    env = FakeEnv()
    source_file = sys.argv[1]
    source_parts = os.path.splitext(source_file)
    target = source_parts[0] + "_test" + source_parts[1]
    env["SOFTWARE_IMAGE"] = sys.argv[2]
    env["CHIPSET"] = sys.argv[3]

    bldcfg_builder([target], [source_file], env)

if __name__ == "__main__":
    _test()
