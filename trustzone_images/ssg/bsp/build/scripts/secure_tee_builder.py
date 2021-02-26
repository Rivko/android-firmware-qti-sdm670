
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to build a Secure TEE
#
# Copyright 2015,2016 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tz/1.0.2/bsp/build/scripts/secure_tee_builder.py#1 $
#  $DateTime: 2018/02/06 03:00:17 $
#  $Author: pwbldsvc $
#  $Change: 15399933 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
#
#============================================================================
import os
import sys
import platform
import fnmatch
import SCons.Script
#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
DEFAULT_APP_SPLITBIN_COUNT = 7

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
  return True

def generate(env):
  secure_tee_generate(env)

#============================================================================
# Add the Secure TEE Builder to the environment. Also Load other builders
# that are needed for secure app compilation via LoadToolScript helper.
#============================================================================
def secure_tee_generate(env):
  env.AddMethod(secure_tee_builder, "SecureTEEBuilder")
  env.AddMethod(get_object_file_name, "GetObjectFileName")
  env.AddMethod(secure_tee_lib_builder, "SecureTEELibBuilder")

  tools = [
    # load the llvm builder
    '${BUILD_ROOT}/tools/build/scons/scripts/llvm.py',
    '${BUILD_ROOT}/tools/build/scons/scripts/apps_defs.py',
    '${BUILD_ROOT}/sectools/sectools_builder.py',
    '${BUILD_ROOT}/core/bsp/build/scripts/mbn_builder.py',
    '${BUILD_ROOT}/ssg/bsp/build/scripts/get_dependency_builder.py',
    '${BUILD_ROOT}/core/bsp/build/scripts/scl_builder.py',
    '${BUILD_ROOT}/ssg/bsp/build/scripts/pil_splitter_builder.py',
  ]
# The tools get loaded as part of init_build_config. Loading them here could cause
# a race condition where these tools get loaded prior to init_build_config and may thus
# miss some of the env variables populated like the USES_* variables. MBN builder does
# reference USES_MBN* variables that get populated in init_build_config
  env.Replace(IMAGE_TOOLS = tools)

#============================================================================
# secure_tee_builder:
# this builder takes in the list of source files, include file paths,
# the metadata dictionary and the desired image name to build the
# secure application. The builder takes key word arguments as optional
# parameters. The key word arguments currently processed are:
# stack_size, heap_size, sec_xml_config, user_libs
# The Node object of the final builder is returned
# back so that the caller can add it as a dependency if needed.
#============================================================================
def secure_tee_builder(env, sources, includes, image, **kwargs):
  env = env.Clone()
  return_list = []

  deploy_variants = kwargs.get('deploy_variants', ['DO_NOT_SHIP'])
  

  env.Append(CPPPATH = includes)
  env.Append(CPPPATH = ['#../../core/api/services/',
                        '#../../core/securemsm/trustzone/qsee/mink/include/',]
            )

  env.Replace(APP_IMAGE_NAME = image)
  env.Append(CCFLAGS = '-fPIC')
  env.Append(LINKFLAGS = ' -shared -Bsymbolic ')

  # The app scons environment is expected to define the OUT_DIR path
  buildPath = '${OUT_DIR}/${SHORT_BUILDPATH}/' + image

  # Compile sources to objects then we can control where the .o files end up
  sobjects = compile_sources(env, buildPath, sources)

  libs = kwargs.get('user_libs',[])

  if not env.OfftargetTesting():
    # The libclang_rt.builtins-arm.a is common for both armv7 and armv8
    libs.insert(0, env.File('${LLVMLIB}/libclang_rt.builtins-arm.a'))

  elf = env.Program(buildPath, source=sobjects, LIBS=libs)
  return_list.extend(elf)
  # For offtarget testing, we don't have special linker scripts, mbns,
  # pil splitting, sectools signing, or deploying, so we can just return
  # the elf
  if env.OfftargetTesting():
    return return_list
  scl = env.SclBuilder(buildPath, '${BUILD_ROOT}/ssg/securemsm/trustzone/qsapps/teetest/teetest/build/' + image + '.ld')
  env.Depends(elf, scl)

  image_map = env.SideEffect(buildPath + '.map', elf)
  env.Clean(elf, image_map)

  pbn = env.InstallAs(buildPath + ".pbn", elf)

  install_root = '${MBN_ROOT}'
  install_unsigned_root = env.SectoolGetUnsignedInstallPath(install_base_dir = install_root)
  env.Replace(MBN_FILE = os.path.join(install_unsigned_root, image))

  mbn = env.MbnBuilder( buildPath,
                        pbn, IMAGE_TYPE=image, MBN_TYPE="elf",
                        IMAGE_ID=4, FLASH_TYPE="sdcc")
  return_list.extend(mbn)

  # Sectools signing

  default_config = '${BUILD_ROOT}/sectools/config/integration/${SECIMAGE_CONFIG}'
  soc_hw_version = SCons.Script.ARGUMENTS.get("SOC_HW_VERSION")
  soc_vers = SCons.Script.ARGUMENTS.get("SOC_VERS")

  secimage_xml_config = kwargs.get('sec_xml_config', default_config)
  
  app_id = kwargs.get('user_app_id', env.GetAppIDByImageName(image))

  pil_base_dir = '${BUILD_ROOT}/build/ms/bin/PIL_IMAGES/SPLITBINS_${QC_SHORT_BUILDPATH}'

  sectools_signed_mbn = env.SectoolBuilder(
                            target_base_dir = '${OUT_DIR}/${SHORT_BUILDPATH}',
                            source = mbn,
                            sign_id = 'tz_tee',
                            app_id = app_id,
                            msmid = env['MSM_ID'],
                            sectools_install_base_dir = install_root,
                            install_file_name = image + ".mbn",
                            config = secimage_xml_config,
                            # Creates duplicate, unused splitbins but ALSO
                            # creates the directories they reside in (which
                            # PilSplitterBuilder does not do). Can remove this
                            # if the proper mkdir command is added.
                            pilsplitter_target_base_dir = None,
                            soc_hw_version = soc_hw_version,
                            soc_vers = soc_vers
                        )
  return_list.extend(sectools_signed_mbn)
  env.Depends(sectools_signed_mbn, mbn)

  # Build files for PIL driver
  split_bin_count = kwargs.get('split_bin_count',DEFAULT_APP_SPLITBIN_COUNT)
  pil_bin_list = env.PilSplit([sectools_signed_mbn, mbn],pil_base_dir, split_bin_count)
  return_list.extend(pil_bin_list)
  
  # default apps deployment (if 'deploy_sources' is not given):
  # mbn & pil images -> given deploy variants (default DO_NOT_SHIP)
  default_deployables = [sectools_signed_mbn, mbn, pil_bin_list]
  deployables = kwargs.get('deploy_sources',default_deployables)
  
  env.Deploy(deployables, deploy_variants=deploy_variants, skip_check=True)

  # elfs & maps are always saved to DO_NOT_SHIP. If they are desired in external
  # dirs, they must be passed in by the caller. sym files are not explicitly
  # created by this builder & thus must be deployed from the app's SConscript.
  env.DeployInternal([elf, image_map])

  return return_list


#this helper function helps return 3 parameters about the object file that is
#getting compiled. It determines the name and relative path of the object file
#from the path and name of source file being compiled. It also returns the
#extension of the source file name. Some apps include '.o' files in the list
#of sources. We want to add these object files directly to list of objects
#since SCons Object builder does not take an object file. Moreover, we also
#return back the status of presence of the source file, to determine if
#the file is stripped or not.
def get_object_file_name(env, spath):
  #source files can be of the form a.c, ../x/a.c, b/a.c.
  #in each of above cases, we want objects to be stored as
  #a.o, x/a.o, b/a.o
  if isinstance(spath, str):
    sstr = spath
    sourceNodeRepr = env.File(spath)
  elif isinstance(spath, list):
    print '...Error: get_object_file_name does not process list!! %s' % (str(spath))
    exit(1)
  else:
    sourceNodeRepr = spath

  spath_str = sourceNodeRepr.path
  split_pattern = env.subst('${BUILD_ROOT}/')
  if platform.system() == 'Windows':
    split_pattern = split_pattern.replace('/', '&').split(':')[-1]
    spath_str = spath_str.split(':')[-1].replace(os.sep, '&')
    sstr = spath_str.split(split_pattern)[-1]
    sstr = sstr.replace('&', os.sep)
  else:
    sstr = spath_str.split(split_pattern)[-1]

  #split at qsapps, otherwise path may become too long for windows to create object file
  sstr = sstr.split('qsapps' + os.sep)[-1]
  name, extension = os.path.splitext(sstr)

  #print 'Returning name and extension = %s %s from get_object_file_name' % (name, extension)
  #also return if the source file exists or we need to look for precompiled object file
  return (name, extension, sourceNodeRepr.exists())

#this is the helper function that helps build libraries that are used by
#apps to compose and link with their images. Using the secure app lib builder,
#we easily control the parameters like library name format, destination of libs and
#lib artifacts.
#archive_objs has special behavior regarding its behavior:
#  exists in kwargs -> use that value to decide to archive or not
#  doesn't exist in kwargs -> use env['SECURE_APP_BUILDER_ARCHIVE_OBJS']
def secure_tee_lib_builder(env, includes, sources, libname, deploy_sources=None, deploy_variants=None, deploy_lib=None, **kwargs):
  if deploy_variants is None:
    deploy_variants = ['DO_NOT_SHIP']
  env = env.Clone()
  env.Replace(LIBNAME = libname)
  env.Append(CCFLAGS = ' -fPIC -ffunction-sections ')
  env.Append(CPPPATH = includes)

  if 'USES_CLEAN_PACK' in env:
    lib = env.File('${LIB_OUT_DIR}/' + libname + '${LIBSUFFIX}')
    if not lib.exists():
      print('...Error: lib %s does not exist' % (lib.rstr()))
      sys.exit(1)
  else:
    install_dir = '${LIB_OUT_DIR}'
    objlist = compile_sources(env, install_dir, sources)
    archive_objs = kwargs.get('archive_objs',env.get('SECURE_APP_BUILDER_ARCHIVE_OBJS',True))
    if archive_objs:
      lib = env.Library(os.path.join(install_dir, libname), objlist)
    else:
      lib = objlist
    if env.GetOption('cleanpack'):
      env.NoClean(lib)

  if deploy_sources:
    env.Deploy(deploy_sources, deploy_variants=deploy_variants, skip_check=True)
  if deploy_lib:
    env.Deploy(lib, deploy_variants=deploy_variants, skip_check=True)
  return lib

#this is the helper function that compiles the sources into object files.
#If the source files exist, they are always used for compilation.
#if the build is stripped of sources, the pre-built object files are
#used instead. To make the code and process easy, the list of prebuilt
#object files is gathered. If sources are present, then the the
#corresponding object file is removed from the list of prebuilt objects
#gathered earlier. The cummulative list of sources and objects is
#presented to the builder to use for compilation
def compile_sources(env, dest_dir, sources):
  sobjects = []

  for source in env.Flatten(sources):
    objname, extension, exists = get_object_file_name(env,source)
    if exists:
      if extension.lower() == '.o':
        obj = source
      else:
        obj = env.Object(os.path.join(dest_dir,'objects',objname), source)
      sobjects.append(obj)
      if env.GetOption('cleanpack'):
        env.NoClean(obj)
    else:
      print "compile_sources can't find file {}".format(env.RealPath(str(source)))
      #raise #TODO: what do we do now?
  return sobjects

