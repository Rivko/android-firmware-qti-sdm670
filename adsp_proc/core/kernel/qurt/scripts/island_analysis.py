#===============================================================================
#                    Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#===============================================================================
import os, sys

#----------------------------------------------------------------------------
# Hooks for SCons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('island_analysis')

def generate(env):
    island_analysis_generate(env)
        
def island_analysis_generate(env):
    env.AddMethod(island_analysis_builder_wrapper, "IslandAnalysis")
    island_analysis_bld = env.Builder(action = island_analysis_builder)
    env['BUILDERS']['IslandAnalysisInternalBuilder'] = island_analysis_bld

def island_analysis_builder_wrapper(env, target, source, libdirs=[], ignorelist=[],pass_chk=0):
    env.Replace(ISLAND_ANALYSIS_LIBDIRS = libdirs)
    env.Replace(ISLAND_ANALYSIS_IGNORELIST = ignorelist)
    env.Replace(ISLAND_ANALYSIS_PASS_CHK = pass_chk)
    return env.IslandAnalysisInternalBuilder(target, source)

def island_analysis_builder(target, source, env):
    libdirs = env.get('ISLAND_ANALYSIS_LIBDIRS')
    ignorelist = env.get('ISLAND_ANALYSIS_IGNORELIST')
    elf = 0
    pass_chk = env.get('ISLAND_ANALYSIS_PASS_CHK')

    if env.get('LIST_EXTREF_PY') == None:
        env.Replace(LIST_EXTREF_PY = "${BUILD_ROOT}/core/kernel/qurt/scripts/list_extref.py")
    if int(env.get('HEXAGON_RTOS_RELEASE').split(".")[0]) >= 7:
        env.Replace(IA_OBJDUMP = "${QDSP6BIN}/hexagon-llvm-objdump${EXE_EXT}")
    else:
        env.Replace(IA_OBJDUMP = "${QDSP6BIN}/hexagon-objdump${EXE_EXT}")
    analysis_cmd = "python ${LIST_EXTREF_PY} -v -e ${IA_OBJDUMP} "
	
    for src in source:
        if str(src).endswith('.o'):
            analysis_cmd += "-o "+str(src)+" "
        elif str(src).endswith('.elf'):
            analysis_cmd += "-x "+str(src)+" "
            elf = src
        else:
            analysis_cmd += "-s "+str(src)+" "
    if elf:
       csv = str(target[1])
       analysis_cmd += "-c "+str(csv)+" "
	   
    for libdir in libdirs:
        analysis_cmd += "-l "+libdir+" "
    for ilist in ignorelist:
        analysis_cmd += "-i "+ilist+" "

    analysis_cmd = env.subst(analysis_cmd)
    analysis_cmd = analysis_cmd+" > "+str(target[0])

    print analysis_cmd
    #std_out, std_err, return_val = env.ExecCmds(analysis_cmd)
    extref = os.system(analysis_cmd)

	#   pass_chk variable tells us if we need to fail the build in case number of external references is more than 0.
    if(pass_chk > 0):
        if(extref > 0):
           print "ERROR : This build is enabled with a feature to fail the build compilation in case of any unaccounted \
DDR symbol references from island code. For list of DDR symbols being referenced check 'Undefined Island Symbols' section in \
island_analysis.txt for more details. These symbols either need to be added to Island sections or whitelisted using\
following scons API 'env.AddSymbolToIslandWhitelist(tags, node, [list of symbols])'"
           sys.exit(1)
     #return target
