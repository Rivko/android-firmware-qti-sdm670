#==============================================================================
# SCONS Tool for Build Component Utilities.
#
# Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
# $Header: //components/rel/dspbuild.adsp/5.0/scripts/build_utils.py#1 $
#
#==============================================================================

'''
Contains utility scripts for the build component.

* FindConfigFiles(config_file_names)

  Returns a path to the specified config files.

  The 'config_file_names' parameter may be a single string, or a list, and
  may contain wild-card characters similar to those used by the glob function.
  The parameter should not contain any path information, just file names.

  The return value is a list of SCons file nodes, one for each config file
  found.
'''

import os

def generate(env):
    env.AddMethod(find_config_files, "FindConfigFiles")
    env.AddMethod(find_config_paths, "FindConfigPaths")
    env.AddMethod(add_rcinit_playlist_params,"AddRcinitPlaylistParams")
    env.AddMethod(add_rcinit_playbook_params,"AddRcinitPlaybookParams")
    env.AddMethod(init_target_variation, "InitTargetVariation")


def exists(env):
    return env.Detect('build_utils')


#-------------------------------------------------------------------------------
# Find the specified source files
#
def find_sources(env, sources, variant_name):

    #---------------------------------------------------------------------------
    # Legacy methods had files in the following locations:
    #
    #     build/ms
    #     build/bsp/modem_proc_img/build
    #
    # Then some of these files were moved to the config directory, and
    # sometimes in various subdirectories of the config directory, and
    # then copied to build/ms or build/bsp/modem_proc_img/build.
    #
    # Now we simply want to return the location of these files and access
    # the files directly from their current locations, which could be any
    # of the above.
    #
    # The search order in this script emulates the search order of the
    # copy algorithm, but we must first look in the legacy build/ms
    # and build/bsp/modem_proc_img/build directories to find files that
    # were never moved to the config directory.
    #
    # The copy algorithm also had two phases.  In the first phase, config
    # files were in flat directories.  In the "new_config" phase, these
    # directories could be nested in a structure that would allow
    # "overlaying" the directory tree on top of the build component.  I.e.
    # the location of a file within this directory tree showed where the
    # file should be copied to in the build directory tree.
    #
    # Since we no longer copy the files, we don't care what this directory
    # structure is, but we still need to locate files within this directory
    # structure.
    #
    # We know to use the "new_config" model if the config/default directory
    # is present.
    #
    # The search order is:
    #
    #     Never nested:
    #
    #         build/ms
    #         build/bsp/modem_proc_img/build
    #         config
    #
    #     Nested if new_config:
    #
    #         config/default
    #         config/<chipset>
    #         config/<flavor>
    #         config/<purpose>
    #         config/<chipset>.<flavor>
    #         config/<chipset>.<flavor>.<purpose>
    #
    # The last instance of a file found is the one that we return.
    #---------------------------------------------------------------------------

    def srch_dir (env, src_dict, src_dir, sources):
        # 'wc' means the string can have wildcards in it.
        for src_wc in sources:
            for src in env.GlobFiles(os.path.join(src_dir, src_wc)):

                # A dictionary is used so that files deeper in the directory
                # structure can replace files of the same name higher in the
                # directory structure.
                src_dict[os.path.basename(src)] = src

    # End of srch_dir()

    # Search paths

    never_nested_search_path = [
         "${BUILD_ROOT}/build/ms",
         "${BUILD_ROOT}/build/bsp/modem_proc_img/build",
         "${BUILD_ROOT}/build/config"
    ]

    nested_if_new_config_search_path = [
         "${BUILD_ROOT}/build/config/default",
         "${BUILD_ROOT}/build/config/default/images/{image_name}",
         "${BUILD_ROOT}/build/config/{chipset}",
         "${BUILD_ROOT}/build/config/{chipset}/images/{image_name}",
         "${BUILD_ROOT}/build/config/{flavor}",
         "${BUILD_ROOT}/build/config/{flavor}/images/{image_name}",
         "${BUILD_ROOT}/build/config/{purpose}",
         "${BUILD_ROOT}/build/config/{purpose}/images/{image_name}",
         "${BUILD_ROOT}/build/config/{chipset}.{flavor}",
         "${BUILD_ROOT}/build/config/{chipset}.{flavor}/images/{image_name}",
         "${BUILD_ROOT}/build/config/{chipset}.{flavor}.{purpose}",
         "${BUILD_ROOT}/build/config/{chipset}.{flavor}.{purpose}/images/{image_name}"
    ]

    # A dictionary is used so that files deeper in the directory
    # structure can replace files of the same name higher in the
    # directory structure.
    src_dict = {}

    # Do we use the "new_config" algorithm (i.e. nested directories)?.

    do_nested_searches = env.PathExists("${BUILD_ROOT}/build/config/default/ms")

    # Split variant name into it consitituent parts
    chipset, flavor, purpose = variant_name.split('.')

    # Non-nested searches
    for directory in never_nested_search_path:
        directory = env.subst(directory)
        search_dir = directory.format(chipset=chipset,
                                      flavor=flavor,
                                      purpose=purpose)
        srch_dir(env, src_dict, search_dir, sources)

    # Find the image name for per-image config files
    image_name = ''
    if 'IMAGE_NAME' in env:
        image_name = env['IMAGE_NAME'].lower()
    else:
        env.PrintInfo("IMAGE_NAME not in env")

    # Now search possibly nested directories
    for directory in nested_if_new_config_search_path:
        directory = env.subst(directory)
        search_dir = env.subst(directory.format(chipset=chipset,
                                                flavor=flavor,
                                                purpose=purpose,
                                                image_name=image_name))
        if do_nested_searches:
            for root, dirs, files in os.walk(search_dir):
                srch_dir(env, src_dict, root, sources)
        else:
            srch_dir(env, src_dict, search_dir, sources)

    return src_dict.values()

# End of find_sources()


#-------------------------------------------------------------------------------
# Find the specified Config files
#
def find_config_files(env, config_file_names, variant_name=None):

    if type(config_file_names) is not list:
        config_file_names = [config_file_names]
        print "Config file names are " + str(config_file_names)

    if variant_name == None:
        # Variant name is used to find the specific config files for the
        # specified variant.
        variant_name = os.environ.get('VARIANT_NAME', '')

        if len(variant_name) == 0:
            raise Exception("VARIANT_NAME must be defined in the environment.")

    # Find the list of source files to copy

    src_list = find_sources(env, config_file_names, variant_name)

    src_list = [x.replace('\\', '/') for x in src_list]

    env.PrintInfo("env.FindConfigFiles() returning: " + ' '.join(src_list))

    return src_list

# End of find_config_files


#-------------------------------------------------------------------------------
# Return a list of search paths in which config files might be located
#
def find_config_paths(env, variant_name=None):

    if variant_name == None:
        # Variant name is used to find the specific config files for the
        # specified variant.
        variant_name = os.environ.get('VARIANT_NAME', '')

        if len(variant_name) == 0:
            raise Exception("VARIANT_NAME must be defined in the environment.")

    # Wild-card search paths - first path found wins.
    wc_search_paths = [
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}.{purpose}/images/{image_name}",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}.{purpose}/ms",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}.{purpose}/bsp/{image_root_rel}",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}.{purpose}",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}/images/{image_name}",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}/ms",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}/bsp/{image_root_rel}",
        "${BUILD_ROOT}/build/config/{chipset}.{flavor}",
        "${BUILD_ROOT}/build/config/{purpose}/images/{image_name}",
        "${BUILD_ROOT}/build/config/{purpose}/ms",
        "${BUILD_ROOT}/build/config/{purpose}/bsp/{image_root_rel}",
        "${BUILD_ROOT}/build/config/{purpose}",
        "${BUILD_ROOT}/build/config/{flavor}/images/{image_name}",
        "${BUILD_ROOT}/build/config/{flavor}/ms",
        "${BUILD_ROOT}/build/config/{flavor}/bsp/{image_root_rel}",
        "${BUILD_ROOT}/build/config/{flavor}",
        "${BUILD_ROOT}/build/config/{chipset}/images/{image_name}",
        "${BUILD_ROOT}/build/config/{chipset}/ms",
        "${BUILD_ROOT}/build/config/{chipset}/bsp/{image_root_rel}",
        "${BUILD_ROOT}/build/config/{chipset}",
        "${BUILD_ROOT}/build/config/default/images/{image_name}",
        "${BUILD_ROOT}/build/config/default/ms",
        "${BUILD_ROOT}/build/config/default/bsp/{image_root_rel}",
        "${BUILD_ROOT}/build/config/default",
        "${BUILD_ROOT}/build/config",
        "${BUILD_ROOT}/build/ms",
        "${BUILD_ROOT}/build/bsp/{image_root_rel}",
    ]

    # Split variant name into it consitituent parts
    chipset, flavor, purpose = variant_name.split('.')

    # Get the image root relative path
    image_root_rel = os.path.relpath(env['IMAGE_ROOT'],
                                     os.path.join(env['BUILD_ROOT'],
                                                  'build/bsp'))

    # Find the image name for per-image config files
    image_name = ''
    if 'IMAGE_NAME' in env:
        image_name = env['IMAGE_NAME'].lower()
    else:
        env.PrintInfo("IMAGE_NAME not in env")

    # Build list of interpolated search paths
    search_paths = []

    for directory in wc_search_paths:
        directory = env.subst(directory)
        directory = directory.format(chipset=chipset,
                                     flavor=flavor,
                                     purpose=purpose,
                                     image_name=image_name,
                                     image_root_rel=image_root_rel)
        directory = directory.replace('\\', '/')
        if os.path.exists(directory):
            search_paths.append(directory)

    env.PrintInfo("env.FindConfigPaths() returning: " + ' '.join(search_paths))

    return search_paths

# End of find_config_paths

def add_rcinit_playlist_params(env, build_tags, image_units):
    # FOR SCONS TOOL EMITTERS TO PLACE OUTPUT PROPERLY
    if not os.path.exists(env.RealPath('${SHORT_BUILDPATH}')):
        if os.makedirs(env.RealPath('${SHORT_BUILDPATH}')):
            raise
    
    # NEVER POLLUTE ENV CONSTRUCTION ENVIRONMENT WHICH GETS INHERITED
    playlist_env = env.Clone()

    # PLAYLIST OUTPUT THIS LIBRARY
    rcinit_out_rcpl = playlist_env.RealPath('${SHORT_BUILDPATH}/rcinit.rcpl')
    playlist_env.AddRCInitPlaylist(build_tags, rcinit_out_rcpl)
    playlist_env.AddArtifact(build_tags, rcinit_out_rcpl)
    # Manage explicit detail outside of AU
    playlist_env.Depends(build_tags, rcinit_out_rcpl)
    # Manage explicit detail outside of AU
    image_units.append(rcinit_out_rcpl)

    # PLAYBOOK TXT OUTPUT THIS LIBRARY
    rcinit_out_txt = playlist_env.RealPath('${SHORT_BUILDPATH}/rcinit_output_log.txt')
    playlist_env.AddRCInitPlaybook(build_tags, rcinit_out_txt, None)
    playlist_env.AddArtifact(build_tags, rcinit_out_txt)
    # Manage explicit detail outside of AU
    playlist_env.Depends(rcinit_out_txt, rcinit_out_rcpl)
    # Manage explicit detail outside of AU
    image_units.append(rcinit_out_txt)
# End of add_rcinit_playlist_params


def add_rcinit_playbook_params(env, build_tags, image_units, image_objs, rcinit_playlist):
    # FOR SCONS TOOL EMITTERS TO PLACE OUTPUT PROPERLY
    if not os.path.exists(env.RealPath('${SHORT_BUILDPATH}')):
        if os.makedirs(env.RealPath('${SHORT_BUILDPATH}')):
            raise
    
    # Required for Compilation of Playbook
    env.RequirePublicApi(['DEBUGTOOLS', 'DAL', 'KERNEL', 'SERVICES'])
    env.RequireRestrictedApi(['DEBUGTOOLS', 'TMS_RESTRICTED'])
    
    # NEVER POLLUTE ENV CONSTRUCTION ENVIRONMENT WHICH GETS INHERITED
    playbook_env = env.Clone()
    
    # PLAYBOOK OUTPUT, IMAGE BUILD DIRECTORY
    rcinit_out_c = playbook_env.RealPath('${SHORT_BUILDPATH}/rcinit_autogen.c')
    # FROM INTERMEDIATES
    playbook_env.AddRCInitPlaybook(build_tags, rcinit_out_c, rcinit_playlist)
    rcinit_objs = env.AddObject(build_tags, rcinit_out_c)
    playbook_env.Depends(rcinit_out_c, rcinit_playlist)
    image_objs.append(rcinit_objs) # ADD PLAYBOOK AS AN IMAGE UNIT

    # PLAYBOOK OUTPUT, RCINIT_OUTPUT_LOG.TXT
    rcinit_out_txt = playbook_env.RealPath('${SHORT_BUILDPATH}/rcinit_output_log.txt')
    playbook_env.AddRCInitPlaybook(build_tags, rcinit_out_txt, None)
    playbook_env.AddArtifact(build_tags, rcinit_out_txt)
    playbook_env.Depends(build_tags, rcinit_out_txt)
    playbook_env.Depends(rcinit_out_txt, rcinit_out_c)
    image_units += [rcinit_out_txt] # ADD PLAYBOOK AS AN IMAGE UNIT
# End of add_rcinit_playbook_params

def init_target_variation(env):
   # import pdb; pdb.set_trace()
   env.Replace(IMAGE=os.environ['IMAGE'])
   if 'BUILD_SCRIPTS_XML_ROOT' in env:
       xml_config = env['BUILD_SCRIPTS_XML_ROOT'] + "/xml_config.py"
       xml_variation = env['BUILD_ROOT'] + "/build/config/default/target_variation.xml"
       if env.PathExists(xml_config) and env.PathExists(xml_variation):
          env['TARGET_CONFIGURATOR'] = env['BUILD_SCRIPTS_XML_ROOT'] + '/xml_config.py'
