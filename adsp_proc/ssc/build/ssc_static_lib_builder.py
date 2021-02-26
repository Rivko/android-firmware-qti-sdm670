#===============================================================================
#
# @file ssc_static_lib_builder.py
#
# GENERAL DESCRIPTION
#   Builder for SSC Static Libraries
#
# Copyright (c) 2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# $Header: //components/rel/ssc.slpi/3.2/build/ssc_static_lib_builder.py#2 $
# $DateTime: 2018/12/03 11:39:17 $
# $Change: 17757034 $
#
#===============================================================================
import inspect
import os
import platform
import subprocess
import logging
from itertools import chain

format_str = '%(filename)s(line %(lineno)s)\t%(levelname)s -- %(message)s'
logging.basicConfig(level=logging.ERROR, format=format_str)
logger = logging.getLogger(__name__)

#logger.setLevel(logging.INFO)  #logs info and higher
#logger.setLevel(logging.DEBUG) #logs everything

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
ME               = inspect.getfile(inspect.currentframe())
MY_ROOT          = os.path.dirname(os.path.dirname(ME))
PB_PUBLIC_PATH   = ""
protoc_command   = []
static_sensors   = []

if "Windows" in platform.system() or "CYGWIN" in platform.system():
  protoc_path = os.path.join(MY_ROOT, "tools", "nanopb", "generator-win")
  protoc = os.path.join(protoc_path, "protoc.exe")
  protoc_nano = os.path.join(protoc_path, "protoc-gen-nanopb.exe")
else:
  protoc_path = os.path.join(MY_ROOT, "tools", "nanopb", "generator")
  protoc = os.path.join(protoc_path, "protoc")
  protoc_nano = os.path.join(protoc_path, "protoc-gen-nanopb")

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
  '''
  exists
  '''
  return env.Detect('ssc_pb')

def generate(env):
  '''
  generate
  '''
  env.AddUsesFlags('USES_SSC_STATIC_LIB_BUILDER')
  env.AddMethod(add_ssc_su, 'AddSSCSU')

  env.AddUsesFlags('USES_NANOPB')
  env.AddMethod(proto_buf_prep, 'ProtoBufPrep')
  env.AddMethod(compile_ssc_pb, 'CompilePB')

  env.AddUsesFlags('USES_STATIC_SENSORS_LIST_GEN')
  env.AddMethod(generate_static_sensor_list, 'GenerateStaticSensorList')

  return None

#==============================================================================
# proto_buf_prep
# TODO - publish separate lists of PUBLIC and PRIVATE proto paths
#==============================================================================
def proto_buf_prep(env):
  global PB_PUBLIC_PATH
  global protoc
  global protoc_command

  pb_paths = []
  PB_PUBLIC_PATH   = env.subst('${SSC_PB_INC}')
  if not os.path.exists(PB_PUBLIC_PATH):
    os.mkdir(PB_PUBLIC_PATH)

  for root, dirs, files in os.walk(env.subst('${SSC_ROOT}')):
    for file in files:
      if file.endswith(".proto"):
        if 'SSC_TARGET_X86' in env['CPPDEFINES'] or "tools" not in root:
          pb_paths.append(root)
          break
  proto_paths = ["--proto_path=%s" %i for i in pb_paths]
  protoc_command = [protoc, "--plugin=" + protoc_nano]
  protoc_command += proto_paths
  logger.debug("protoc_command =\n" + "\n".join(["  %s" % i for i in protoc_command]))

#==============================================================================
# add_ssc_su
#==============================================================================
def add_ssc_su(env, caller,
               # List of folders that contains the source files to be compiled
               flavor = ["src"],
               core_public_api = ['DEBUGTOOLS','KERNEL'], # for debug printf
               ssc_protected_api = [],
               clean_pack_list = [],
               # List of additional flags for the compiler. Will be added to
               # env['CFLAGS']
               cflags = [],
               # Use either island_sections or add_island_files to indicate
               # island content for the SU.
               # Both options at one time is unsupported.
               island_sections = [],
               add_island_files = False,
               register_func_name = "",
               # True or False; "Source", "Binary", or "Shared"
               binary_lib = True,
               # How many copies of this library to register with SEE
               registration_cnt = 1,
               # String: Diag SSID to use. One of: MSG_SSID_SNS MSG_SSID_SNS_FRAMEWORK, 
               # MSG_SSID_SNS_PLATFORM, MSG_SSID_SNS_SENSOR_INT, MSG_SSID_SNS_SENSOR_EXT
               diag_ssid = '',
             ):
  global PB_PUBLIC_PATH
  global static_sensors

  SU_ROOT = os.path.dirname(os.path.dirname(caller))
  SU_NAME = os.path.splitext(os.path.split(caller)[1])[0]
  SRCPATH = [os.path.join(SU_ROOT, ingredient) for ingredient in flavor]
  INCPATH = [os.path.join(SU_ROOT, 'inc')]
  PB_PATH = os.path.join(SU_ROOT, 'pb')
  logger.info("%s" % SU_NAME)
  logger.debug("root = %s" % SU_ROOT)
  logger.debug("core_public_api = %s" % core_public_api)
  logger.debug("ssc_protected_api = %s" % ssc_protected_api)
  logger.debug("clean_pack_list = %s" % clean_pack_list)
  logger.debug("island_sections = %s" % island_sections)
  logger.debug("register_func_name = %s" % register_func_name)

  #-------------------------------------------------------------------------------
  # Build settings
  #-------------------------------------------------------------------------------
  env.Append(CFLAGS = cflags)

  if diag_ssid is not '':
    diag_ssid = '-DSNS_DIAG_SSID='+diag_ssid;
    env.Append(CPPDEFINES = diag_ssid)

  if 'SSC_TARGET_X86' in env['CPPDEFINES']:
    env.Append(CFLAGS = '-Wno-missing-field-initializers')

  #-------------------------------------------------------------------------------
  # External dependencies
  #-------------------------------------------------------------------------------
  if core_public_api and 'SSC_TARGET_HEXAGON' in env['CPPDEFINES']:
    env.RequirePublicApi(core_public_api, area='core')

  #-----------------------------------------------------------------------------
  # Source Path
  #-----------------------------------------------------------------------------
  for folder in SRCPATH :
    if not os.path.isdir(folder):
      if (type(binary_lib) is bool and binary_lib) or "Binary" == binary_lib:
        env.PrintInfo("Ignoring flavor %s for Binary lib", str(flavor))
      else:
        env.PrintError("For flavor %s: Path %s not found" %(str(flavor), folder))
        env.Exit()

  INCPATH += SRCPATH
  env.VariantDir('${BUILDPATH}', SU_ROOT, duplicate=0)

  #-----------------------------------------------------------------------------
  # Internal dependencies
  #-----------------------------------------------------------------------------
  env.RequirePublicApi('SNS_INCLUDES', area='ssc')
  env.PublishPrivateApi(SU_NAME + '_API', INCPATH)
  if ssc_protected_api:
    env.RequireProtectedApi(ssc_protected_api)

  #-------------------------------------------------------------------------------
  # Source files
  #-------------------------------------------------------------------------------
  SRCFILES =  [env.FindFiles(['*.c','*.S'], folder) for folder in SRCPATH]
  ISLAND_SRCFILES = [env.FindFiles(['*_island.c'], folder) for folder in SRCPATH]

  #Flatten the above two list of lists
  SRCFILES = list(chain(*SRCFILES))
  ISLAND_SRCFILES = list(chain(*ISLAND_SRCFILES))

  [SRCFILES.remove(f) for f in ISLAND_SRCFILES if f in SRCFILES]

  VARIANT_SRCFILES = [f.replace(SU_ROOT,'${BUILDPATH}') for f in SRCFILES]
  VARIANT_ISLAND_FILES  = [f.replace(SU_ROOT,'${BUILDPATH}') for f in ISLAND_SRCFILES]
  #logger.debug("VARIANT_SRCFILES =\n" + "\n".join(["  %s" % i for i in VARIANT_SRCFILES]))
  #logger.debug("VARIANT_ISLAND_FILES =\n" + "\n".join(["  %s" % i for i in VARIANT_ISLAND_FILES]))

  #-------------------------------------------------------------------------------
  # Protocol Buffer API files
  #-------------------------------------------------------------------------------
  VARIANT_PROTO_SRCFILES = []
  if 'USES_NANOPB' in env:
    if os.path.isdir(PB_PATH):
      PROTO_NAME = SU_NAME.upper() + "_PROTO"
      PROTO_SRCFILES, HDRFILES = env.CompilePB(env['SSC_BUILD_TAGS'], PROTO_NAME, env.FindFiles(['*.proto'],PB_PATH))
      #logger.debug("PROTO_SRCFILES =\n" + "\n".join(["  %s" % i for i in VARIANT_PROTO_SRCFILES]))
      #logger.debug("HDRFILES =\n" + "\n".join(["  %s" % i for i in HDRFILES]))

      # moves generated pb.h files to top level inc folder
      try:
        try:
          [os.remove(f.replace(PB_PATH,PB_PUBLIC_PATH)) for f in HDRFILES]
        except OSError:
          pass      
        [os.rename(f, f.replace(PB_PATH,PB_PUBLIC_PATH)) for f in HDRFILES]
      except OSError:
        [os.remove(f) for f in HDRFILES]
        pass
      VARIANT_PROTO_SRCFILES += [f.replace(SU_ROOT,'${BUILDPATH}') for f in PROTO_SRCFILES]

  #-------------------------------------------------------------------------------
  # Should library be built
  #-------------------------------------------------------------------------------
  should_build = False
  if 'all' in env['SSC_INCLUDE_LIBS'] and SU_NAME not in env['SSC_EXCLUDE_LIBS']:
    should_build = True
  elif SU_NAME in env['SSC_INCLUDE_LIBS']:
    should_build = True
  else:
    env.PrintInfo("SSC: Skipping %s" %SU_NAME)

  #-------------------------------------------------------------------------------
  # Add Libraries to Image
  #-------------------------------------------------------------------------------
  if should_build:
    SU_LIB = os.path.join('${BUILDPATH}', SU_NAME)
    ISLAND_SU_LIB = SU_LIB + "_island"
    SU_PROTO_LIB = SU_LIB + "_PROTO"

    if "Shared" == binary_lib:
      env.PrintInfo("Shared library: %s" %SU_NAME)
      if len(VARIANT_PROTO_SRCFILES) > 0:
        VARIANT_SRCFILES += [f.replace(SU_ROOT,'${BUILDPATH}') for f in PROTO_SRCFILES]
      if len(VARIANT_ISLAND_FILES) > 0:
        VARIANT_SRCFILES += [f.replace(SU_ROOT,'${BUILDPATH}') for f in VARIANT_ISLAND_FILES]
      lib = env.AddSSCSharedLibrary(['SSC_SHARED_LIBS'], SU_LIB, VARIANT_SRCFILES, [])
    elif 'SSC_TARGET_X86' in env['CPPDEFINES']:
        lib = env.AddLibrary(['SSC_SIM'], SU_LIB, VARIANT_SRCFILES)
        if len(VARIANT_ISLAND_FILES) > 0:
          ulib = env.AddLibrary(['SSC_SIM'], ISLAND_SU_LIB, VARIANT_ISLAND_FILES)
        if len(VARIANT_PROTO_SRCFILES) > 0:
          proto_lib = env.AddLibrary(['SSC_SIM'], SU_PROTO_LIB, VARIANT_PROTO_SRCFILES)
    elif (type(binary_lib) is bool and binary_lib) or "Binary" == binary_lib:
      lib = env.AddBinaryLibrary(env['SSC_BUILD_TAGS'], SU_LIB, VARIANT_SRCFILES)
      ulib = env.AddBinaryLibrary(env['SSC_BUILD_TAGS'], ISLAND_SU_LIB, VARIANT_ISLAND_FILES)
      proto_lib = env.AddBinaryLibrary(env['SSC_BUILD_TAGS'], SU_PROTO_LIB, VARIANT_PROTO_SRCFILES)
    else:
      lib = env.AddLibrary(env['SSC_BUILD_TAGS'], SU_LIB, VARIANT_SRCFILES)
      ulib = env.AddLibrary(env['SSC_BUILD_TAGS'], ISLAND_SU_LIB, VARIANT_ISLAND_FILES)
      proto_lib = env.AddLibrary(env['SSC_BUILD_TAGS'], SU_PROTO_LIB, VARIANT_PROTO_SRCFILES)

    #-------------------------------------------------------------------------------
    #island mode
    #-------------------------------------------------------------------------------
    if 'USES_ISLAND' in env and "Shared" != binary_lib:
      # Add all proto libs in island
      env.AddIslandLibrary(env['SSC_BUILD_TAGS'], proto_lib)

      # Add only *_island.c files for the SU in uimg
      if add_island_files:
        if len(island_sections) > 0:
          # Unsupported combination
          logger.debug("Both island sections and island files enabled")
          raise Exception()
        else:
          # only add island files in island memory
          env.AddIslandLibrary(env['SSC_BUILD_TAGS'], ulib)
      # Add all libs for the SU in uimg
      elif '*' in island_sections:
        env.AddIslandLibrary(env['SSC_BUILD_TAGS'], lib)
        if len(VARIANT_ISLAND_FILES) > 0:
          env.AddIslandLibrary(env['SSC_BUILD_TAGS'], ulib)

      # Add uimg tagged sections from all libs for the SU in uimg
      elif len(island_sections) > 0:
        env.AddIslandLibrary(env['SSC_BUILD_TAGS'], lib, island_sections)
        if len(VARIANT_ISLAND_FILES) > 0:
          env.AddIslandLibrary(env['SSC_BUILD_TAGS'], ulib, island_sections)
      else:
        logger.debug("No island files/sections for SU")

    #-------------------------------------------------------------------------------
    # Add register function to static sensor list
    #-------------------------------------------------------------------------------
    if 'USES_STATIC_SENSORS_LIST_GEN' in env and "Shared" != binary_lib:
      if register_func_name:
        if "sns_register_suid_sensor" == register_func_name:
          static_sensors.insert(0, (register_func_name, registration_cnt))
        else:
          static_sensors.append((register_func_name, registration_cnt))

  #-------------------------------------------------------------------------------
  # Remove source files
  #-------------------------------------------------------------------------------
  if clean_pack_list:
    REMOVE_FILES  = env.FindFiles(clean_pack_list, SU_ROOT)
    env.CleanPack(env['SSC_BUILD_TAGS'], REMOVE_FILES)
    #logger.debug("REMOVE_FILES =\n" + "\n".join(["  %s" % i for i in REMOVE_FILES]))

#==============================================================================
# compile_ssc_pb
# Compiles the given .proto files to generate pb.c and pb.h files
#==============================================================================
def compile_ssc_pb(env, tags, proto_name, proto_files):
  #logger.debug("***** compile_ssc_pb for %s *****" % proto_name)
  src_files = []
  hdr_files = []
  if env.IsKeyEnable(tags) is True:
    for file in proto_files:
      try:
        file_path, file_name = os.path.split(file)
        command = protoc_command + ["--nanopb_out=" + file_path] + [file]
        #logger.debug("command =\n" + "\n".join(["  %s" % i for i in command]))

        cmd_str = ' '.join(command)

        print "[PROTOC] " + cmd_str

        subprocess.check_output(command)

        src_files.append(file.replace(".proto", ".pb.c"))
        hdr_files.append(file.replace(".proto", ".pb.h"))

      except:
        if 'SENSORS_DD_DEV_FLAG' not in env:
          env.PrintError("NanoPB Compilation failed: " + file)
          raise
          #raise Exception()
  return src_files, hdr_files

#==============================================================================
# Generates sns_static_sensors.c
#==============================================================================
def generate_static_sensor_list(env, tags):
  global static_sensors
  if env.IsKeyEnable(tags) is True:
    logger.info("generate_static_sensor_list() called with %d sensors" % len(static_sensors))

    #dest = os.path.join(env.subst('${SSC_ROOT}'), 'framework', 'src')
    #if not os.path.isdir(dest) or not os.listdir(dest):
    #  return None

    if len(static_sensors) == 0:
      logger.error("There are no static sensors?!!!")
      return None

    static_sensors_file = os.path.join(env.subst('${SSC_ROOT}'),
                                       'framework', 'src', 'sns_static_sensors.c')
    fo = open(static_sensors_file, "w")
    fo.write("/* Autogenerated file.  Manual modification is pointless. */\n\n")
    fo.write("#include \"sns_rc.h\"\n")
    fo.write("#include \"sns_register.h\"\n")
    fo.write("#include \"sns_types.h\"\n")
    fo.write("\n")
    for reg_func,reg_cnt in static_sensors:
      fo.write("sns_rc %s(sns_register_cb const *register_api);\n" % reg_func)
    fo.write("\nconst sns_register_entry sns_register_sensor_list[] =\n{\n")
    for reg_func,reg_cnt in static_sensors:
      fo.write(" { %s, %i},\n" % (reg_func, reg_cnt))
    fo.write("};\n\n")
    fo.write("const uint32_t sns_register_sensor_list_len = ARR_SIZE(sns_register_sensor_list);\n\n")
    fo.close()
