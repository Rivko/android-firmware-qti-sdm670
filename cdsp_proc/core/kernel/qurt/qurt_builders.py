#=============================================================================
#
#                                    qurt_builders.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
from SCons.Script import *
import SCons.Node.Python
import re
import subprocess
import signal
from threading import Timer

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('qurt_builders')

def generate(env):
   # load builder into enviroment
   cust_config_obj_generate(env)
   obj_dump_generate(env)
   sim_generate(env)
   test_sim_generate(env)
   test_island_analysis_generate(env)
   reloc_generate(env)
   qurt_image_generate(env)
   qurt_srm_generate(env)
   build_param_generate(env)
   config_vars_generate(env)
   const_autogen_generate(env)
   ppend_cust_config_generate(env)
   traptab_generate(env)
   qurt_program_generate(env)
   hex_cfg_generate(env)
   lcs_generate(env)
   qurt_object_scan_generate(env)
   qurt_filefromiterable_generate(env)
   test_lowfootprint_mem_analysis_generate(env)
   mem_budget_action_generate(env)
   return None

#------------------------------------------------------------------------------
# Customer Configuration definition
#------------------------------------------------------------------------------
def cust_config_obj_generate(env):
   CustConfigObjBuilder = Builder(action = 'python ${Q6_RTOS_ROOT}/scripts/qurt_config.py update ${USE_LLVM_OPT} -T ${Q6_TOOLS_ROOT} -o $TARGET $SOURCE',
                       emitter = cust_config_emitter,
                       suffix = '.o',
                       src_suffix = '.xml')
   env['BUILDERS']['CustConfigObj'] = CustConfigObjBuilder

def cust_config_emitter(target, source, env):
   # Add dependencies on scripts
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/Input/cust_config.c")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/Input/build_params.txt")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/__init__.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/build_qurt_config.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/ezxml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/interrupt_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/kernel_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/machine_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/merge.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/parse_build_params.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/parse_spec.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/physpool_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/program_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/qurt_config.py")

   return (target, source)

#------------------------------------------------------------------------------
# Object Dump
#------------------------------------------------------------------------------
def obj_dump_generate(env):
   ObjDumpBuilder = Builder(action = '${OBJDUMP} ${OBJDUMPOPT} $SOURCE > $TARGET',
                       suffix = '.dump',
                       src_suffix = '.pbn')
   env['BUILDERS']['ObjectDump'] = ObjDumpBuilder

#------------------------------------------------------------------------------
# Simulator
#------------------------------------------------------------------------------
def sim_generate(env):
   SimBuilder = Builder(action = "${SIM} -S $TARGET --simulated_returnval $SOURCE ${SIM_RTOS} ${SYMFILE} ${SIM_COSIM} ${SIMOUT} ${SIMERR}",
                       suffix = '.txt')
   env['BUILDERS']['Simulator'] = SimBuilder

#------------------------------------------------------------------------------
# Simulator for testing, timeout after
#------------------------------------------------------------------------------
def test_sim_generate(env):
    TestSimBuilder = Builder(action = test_sim_builder)
    env['BUILDERS']['TestSimulator'] = TestSimBuilder

def kill_proc(proc, timeout):
    timeout["value"] = True
    if ((sys.version_info[0] == 2) and (sys.version_info[1] >= 6)) or (sys.version_info[0] > 2):
        proc.kill()
    else:
        os.kill(proc.pid, signal.SIGKILL)

def test_sim_builder(target, source, env):
    machine=''
    if env.subst("$Q6VERSION") == 'v62' or env.subst("$Q6VERSION") == 'v65' or env.subst("$Q6VERSION") == 'v55' or env.subst("$Q6VERSION") == 'v66':
       machine = '-m'+env.subst("$T32_MCD_ARCH")
    else:
       machine = env.subst("$HEXAGON_MACHINE")

    prof = ""
    if env.subst('$USE_PROFILE') == 'True':
        prof = "--profile"

    if env.subst('$USE_LLVM') == 'True':
       cmd = "%s -S %s --simulated_returnval %s --rtos %s --symfile %s --cosim_file %s %s --dsp_clock 500 %s" % (env.subst("${SIM}"),
          os.path.abspath(str(target[1])), os.path.abspath(str(source[0])), os.path.abspath(str(source[1])), os.path.abspath(str(source[2])), os.path.abspath(str(source[3])), machine, prof)
    else:
        cmd = "%s -S %s --simulated_returnval %s --rtos %s --symfile %s --cosim_file %s %s" % (env.subst("${SIM}"),
          os.path.abspath(str(target[1])), os.path.abspath(str(source[0])), os.path.abspath(str(source[1])), os.path.abspath(str(source[2])), os.path.abspath(str(source[3])), prof)

    if 'CUSTOM_SIM_ARGS' in env:
        cmd += " " + env.subst( env[ 'CUSTOM_SIM_ARGS' ] )

    command = cmd.split()

    if 'TEST_COMMAND_LINE_ARGS' in env:
        command.extend(env.Flatten(['--',env['TEST_COMMAND_LINE_ARGS']]))
    print ' '.join(command)

    if env.subst('$LIVEOUTPUT') and os.name == 'posix' and os.isatty(1):
        #
        # Standard out is a tty.  We are being run interactively on a Linux(?) machine.
        #
        # Start a pty (pseudo terminal) and start the subprocess on that pty
        #
        import pty
        import select
        import signal
        import sys
        import time
        master, slave = pty.openpty()
        sigvalue = signal.SIGINT
        time_to_kill = time.time() + float(env.subst('$TEST_TIMEOUT'))
        simproc = subprocess.Popen(command, stdin=slave, stdout=slave, stderr=slave, close_fds=True, cwd=env.subst("${BUILD_DIR}"))
        outfile = open(str(target[2]), 'w')
        terminate = False
        try:
            while True:
                if time.time() >= time_to_kill:
                    os.kill(simproc.pid, sigvalue)
                    sigvalue = signal.SIGKILL
                    time_to_kill += 5
                #
                #  Wait up to 1 second for data to be available on (master)
                #
                elif select.select([master],[],[],1.0)[0]:
                    #
                    #  Data available on master.  Read it and echo it to stdout and the output file
                    #
                    data = os.read(master, 0x1000)
                    sys.stdout.write(data)
                    outfile.write(data)
                elif terminate:
                    #
                    #  Program terminated.
                    #
                    os.close(slave)
                    os.close(master)
                    outfile.close()
                    if simproc.returncode != 0:
                        if sigvalue != signal.SIGINT:
                            print 'Test failed with timeout'
                            return 'TIMEOUT'            # Did not terminate successfully, and we sent a timeout signal
                    outfile = open(str(target[0]), 'w')
                    if simproc.returncode == 0:
                        print 'Test passed'
                        outfile.write('PASS')
                    else:
                        print 'Test failed'
                        outfile.write('FAIL')
                    outfile.close()
                    return simproc.returncode
                elif simproc.poll() != None:
                    #
                    #  If the subprocess terminated, don't immediately start shutting stuff down.
                    #  There might be residual content available at (master).  Only shut down
                    #   after (master) has run empty.
                    #
                    terminate = True
                else:
                    pass
        except:
            #
            #  Kill the subprocess if we take an exception
            #
            os.kill(simproc.pid, signal.SIGKILL)
            raise

    simproc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=env.subst("${BUILD_DIR}"))

    import time

    timeout = {"value": False}
    starttime = time.time()
    timer = Timer(float(env.subst("${TEST_TIMEOUT}")), kill_proc, [simproc, timeout])
    timer.start()
    stdout, stderr = simproc.communicate()
    timer.cancel()
    endtime = time.time()

    outfile = open(str(target[2]), 'w+')
    outfile.write(stdout)
    outfile.write(stderr)
    outfile.write("Test time: "+str(endtime-starttime))
    outfile.close

    if str(timeout["value"]) == "True":
        return "TIMEOUT"
    else:
        file = open(str(target[0]), 'w+')
        if simproc.returncode == 0:
            file.write('PASS')
            if env.subst('$USE_PROFILE') == 'True':
                gmon_files = ""
                for node in Glob(env.subst("${BUILD_DIR}")+'/gmon.t_[S,i,0-9]*'):
                    gmon_files += " "+str(node)
                prof_cmd = "%s -d %s -o %s %s -F" % (env.subst("${PROF}"), str(source[4]), str(target[3]), gmon_files)
                print prof_cmd
                subprocess.call(prof_cmd.split())
        else:
            file.write('FAIL')
        file.close
        return simproc.returncode

#------------------------------------------------------------------------------
# Simulator for testing, timeout after
#------------------------------------------------------------------------------
def test_island_analysis_generate(env):
    TestIslandAnalysisBuilder = Builder(action = test_island_analysis_builder)
    env['BUILDERS']['TestIslandAnalysis'] = TestIslandAnalysisBuilder

def test_island_analysis_builder(target, source, env):
    size_threshold = env.get('ISLAND_THRESHOLD')
    f = open(str(source[0]), 'r')
    analysis = f.read()
    f.close()

    size = "65535"
    extref = "0"
    for analysisline in analysis.split("\n"):
        undefpattern = re.compile('Total Undefined: (.+){1}')
        sizepattern = re.compile('Total: (.+){1} KB')
        sizematch = sizepattern.match(analysisline)
        undefmatch = undefpattern.match(analysisline)
        if(sizematch):
            size = sizematch.group(1)
        if(undefmatch):
            extref = undefmatch.group(1)

    fail=False
    analysis_result="${BUILD_DIR}/analysis_result"
    file = open(str(target[0]), 'w+')
    if ((float(size) <= float(size_threshold)) and (int(extref) == 0)):
        file.write('PASS')
    else:
        file.write('FAIL')
        fail=True
    file.close

    file = open(str(target[1]), 'w+')
    file.write("Full Island Analysis file found at:\n")
    file.write(os.path.abspath(str(source[0])))
    file.write("\n\nIsland size is "+size+" KB\n")
    if (float(size) > float(size_threshold)):
        file.write("***EXCEEDS THRESHOLD***\n")
    file.write("\nUnaccounted external references: "+extref+"\n")
    if (int(extref) != 0):
        file.write("***EXCEEDS THRESHOLD***\n")
    file.close

    if (fail):
        return -1

#------------------------------------------------------------------------------
# hex cfg
#------------------------------------------------------------------------------
def hex_cfg_generate(env):
    HexCfgBuilder = Builder(action = hex_cfg_builder)
    env['BUILDERS']['HexCfg'] = HexCfgBuilder

    # Register a SCons Scanner for Hexagon Co-Sims listed in .cos file
    cosim_scanner = Scanner( function = scan_cosims, skeys = [ '.cos' ] )
    env.Append( SCANNERS = cosim_scanner )

def scan_cosims (node, env, path):
    """Scan nodes for cosim invocations and return path to cosims
    """
    cosims = set()

    # Pattern to extract path to co-sim
    pattern = re.compile( '^\s*(\S+)\s+.*$' )

    lines = node.get_contents().split( '\n' )
    for line in lines:
        result = pattern.match( line )
        if result:
            cosims.update( env.subst( result.groups() ) )
    return list( cosims )

def hex_cfg_builder(target, source, env):
    timerlib = env.subst('$TimerLib')
    l2viclib = env.subst('$L2vicLib')
    srcfile = open(str(source[0]),'r')
    tempstr = srcfile.read()
    srcfile.close()

    tgtfile = open(str(target[0]),'w+')
    tempstr = tempstr.replace('qtimer.so', timerlib)
    tempstr = tempstr.replace('l2vic.so', l2viclib)

    for src in source[1:]:
        if os.path.isfile( src.path ) and os.path.splitext( src.path )[ 1 ] == '.cos':
            # Argument is *ASSUMED* to be a text file containing list of cosims
            with open( src.path, 'r' ) as ifile:
                # Read cosim listing and substitute SCons environment variables
                tempstr += '\n' + env.subst( ifile.read() )
        elif ('nmi_status_sim' in src.name):
            tempstr = tempstr.replace('nmi_status_sim.so', src.get_abspath())
        else:
            tgtfile.write(src.get_abspath()+'\n')
    tgtfile.write(tempstr)

    tgtfile.close()

#------------------------------------------------------------------------------
# Relocation
#------------------------------------------------------------------------------
def reloc_generate(env):
   RelocBuilder = Builder(action = "${OBJCOPY} --change-start +${RELOC_ADDR} " +
                                 "--change-section-lma .start+${RELOC_ADDR} " +
                                 "--change-section-lma .init+${RELOC_ADDR} " +
                                 "--change-section-lma .text+${RELOC_ADDR} " +
                                 "--change-section-lma .fini+${RELOC_ADDR} " +
                                 "--change-section-lma .rodata+${RELOC_ADDR} " +
                                 "--change-section-lma .data+${RELOC_ADDR} " +
                                 "--change-section-lma .eh_frame+${RELOC_ADDR} " +
                                 "--change-section-lma .ctors+${RELOC_ADDR} " +
                                 "--change-section-lma .dtors+${RELOC_ADDR} " +
                                 "--change-section-lma .bss+${RELOC_ADDR} " +
                                 "--change-section-lma .sdata+${RELOC_ADDR} " +
                                 "--change-section-lma .sbss+${RELOC_ADDR} " +
                                 "$SOURCE $TARGET")
   env['BUILDERS']['Relocate'] = RelocBuilder

#------------------------------------------------------------------------------
# Builder: Deriving build_params.txt
#------------------------------------------------------------------------------
def build_param_generate(env):
   build_param_bld = Builder(action = Action(gen_build_param, cmdstr='Building ${TARGET} ...'), emitter=gen_build_emitter)
   env['BUILDERS']['build_param_builder'] = build_param_bld

def gen_build_emitter(target, source, env):
   # Ignore any sources specified; they were never used anyway.
   #
   # There is only one source needed, and it's a SCons Value node
   #  containing the string contents of the output file.
   #
   # This has the desired effect of rebuilding the output file
   #  any time that its contents would change, and leaving it
   #  unchanged otherwise.
   #
   contents  = ("Q6VERSION="         +env.subst('$Q6VERSION')+"\n")
   contents += ("BUILD_CONFIG_FLAGS="+env.subst('$BUILD_CONFIG_FLAGS')+"\n")
   contents += ("Q6_TOOLS_ROOT="     +os.path.abspath(env.subst('$Q6_TOOLS_ROOT'))+"\n")
   return target, [env.Value(env.subst(contents))]

def gen_build_param(target, source, env):
   target = str(target[0])
   fileHandle = open (target, 'w')
   fileHandle.write(source[0].get_contents())
   fileHandle.close()

#------------------------------------------------------------------------------
# Builder: Prepending cust_config.c
#------------------------------------------------------------------------------
def ppend_cust_config_generate(env):
   cust_config_bld = Builder(action = ppend_cust_config)
   env['BUILDERS']['cust_config_builder'] = cust_config_bld

def ppend_cust_config(target, source, env):
   target = str(target[0])
   tmplt  = str(source[0])
   mcr    = str(source[1])

   fileTarget = open (target, 'w')
   fileTmplt  = open (tmplt, 'r')
   fileMcr    = open (mcr, 'r')

   for line in fileMcr:
      if line.find('THREAD_CONTEXT_TOTALSIZE') != -1:
         fileTarget.write(line)
      elif line.find('QURT_CONFIG_MAX_HTHREAD') != -1:
         fileTarget.write(line)

   for line in fileTmplt:
      fileTarget.write(line)

   fileTarget.close()

#------------------------------------------------------------------------------
# Builder: qurt_config_vars.h
#------------------------------------------------------------------------------
def config_vars_generate(env):
   env['BUILDERS']['ConfigVarsGen'] = Builder(generator = config_vars_generator, emitter = config_vars_emitter)

def config_vars_emitter(target, source, env):
    return target, [env.subst('$INSTALL_DIR/scripts/qurt_config_x.py')]

def config_vars_generator(source, target, env, for_signature):
    return env.subst('python %s genheader -T ${Q6_TOOLS_ROOT} %s' % (str(source[0]), str(target[0])))


#------------------------------------------------------------------------------
# Builder: qurt_config_vars.h
#------------------------------------------------------------------------------
def const_autogen_generate (env):
   import os
   env['QURTNUL'] = os.devnull
   env['BUILDERS']['const_autogen_builder'] = Builder(action = const_autogen_builder)

def const_autogen_builder(target, source, env):
    if env.subst("$Q6VERSION") >= 'v60':
       arglist = env.subst("${SIM} --quiet --pmu_statsfile $QURTNUL "+str(source[0])).split(" ")
    else:
      arglist = env.subst("${SIM} --quiet --pmu_statsfile $QURTNUL --statsfile $QURTNUL "+str(source[0])).split(" ")
    outinc_file = open(str(target[0]), 'w')
    print arglist
    cpp = subprocess.Popen(arglist, stdout=outinc_file)
    val = cpp.wait()
    outinc_file.close()
    if val != 0:
        os.remove(str(target[0]))
    return val
#------------------------------------------------------------------------------
# xml config with paddr option
#------------------------------------------------------------------------------
def qurt_image_generate(env):
    def qib_generator(source, target, env, for_signature):
        interp = env.subst('python ${Q6_RTOS_ROOT}/scripts/qurt-image-build.py')
        tgtname = '-o %s' % target[0]
        srcnames = ' '.join([getattr(s.attributes,'cmdtemplate','%s') % s for s in source])
        physaddr = env.subst('-p ${PADDR}')
        return ' '.join([interp,srcnames,tgtname,physaddr])
    def qib_emitter(target, source, env):
        tmp = env.subst('${SRM_FILENAME}')
        if tmp:
            srcnode = File(tmp)
            srcnode.attributes.cmdtemplate = 'SRM=%s'
            source.append(srcnode)
        if 'QIBSTITCHFILES' in env:
            for x in env.Flatten([env['QIBSTITCHFILES']]):
                if not hasattr(x.attributes, 'cmdtemplate'):
                    x.attributes.cmdtemplate = '--file %s'
                source.append(x)
        return target, source
    QurtImageBuilder = Builder(generator = qib_generator, emitter=qib_emitter)
    env['BUILDERS']['QurtImage'] = QurtImageBuilder

def qurt_srm_generate(env):
    def srm_generator(source, target, env, for_signature):
        cmd = []
        cmd.append('python')
        cmd.append('$SOURCES')
        cmd.append('--output-file')
        cmd.append('$TARGET')
        cmd.append('--lcs-file')
        cmd.append(str(target[0].attributes.lcsfile))
        cmd.append('--map-file')
        cmd.append(str(target[0].attributes.mapfile))
        if env.subst('${SRM_ADDRESS}'):
            cmd.append('--start-address')
            cmd.append('${SRM_ADDRESS}')
        cmd.append('--hexagon-tools')
        cmd.append('${Q6_TOOLS_ROOT}')
        cmd.append('--qurt-location')
        cmd.append('${Q6_RTOS_ROOT}')
        return ' '.join(cmd)
    def srm_emitter(target, source, env):
        target[0].attributes.lcsfile = env.File(str(target[0])+'.lcs')
        target[0].attributes.mapfile = env.File(str(target[0])+'.map')
        env.Clean(target[0], target[0].attributes.lcsfile)
        env.Clean(target[0], target[0].attributes.mapfile)
        source.insert(0, env.subst('${Q6_RTOS_ROOT}/scripts/qurt-srm-build.py'))
        return target, source
    QurtBuildSRM = Builder(generator = srm_generator, emitter = srm_emitter)
    env['BUILDERS']['QurtBuildSRM'] = QurtBuildSRM

#------------------------------------------------------------------------------
# traptab builder
#------------------------------------------------------------------------------
def traptab_generate(env):
   env['BUILDERS']['TrapTabGen'] = Builder(generator = traptab_generator)
def traptab_generator(source, target, env, for_signature):
   return ' '.join(['python ', str(source[0]),'-outtab', str(target[0]), '-inc', str(source[2]), env['CC'],env.subst('$CCFLAGS'),' '.join("-I%s" % w for w in env['CPPPATH']),'-E -x c', str(source[1])])

#------------------------------------------------------------------------------
# QuRT program builder (tricked out replacement for Program)
#
# Allows direct specification of XML files and linker scripts as sources
#  to the QurtProgram.  Also handles a lot of the details regarding how
#  to invoke the linker correctly.
# Installs dependencies on the standard QuRT and tools library files.
#------------------------------------------------------------------------------
def qurt_program_generate(env):
   def qurt_program_generator(source, target, env, for_signature):
      control = env['QPCONTROL']
      cmd = [control['LINK'],
             [control.get('LINKFLAGS',[])],
             '-m'+env['Q6VERSION'],
             '-g',
             control['SMALLDATA'],
             '-nostdlib',
             [['--island-spec', str(source[i])] for i in control['SPECSRCS']],
             [['-T', str(source[i])] for i in control['CMDSRCS']],
             [['--section-start', s] for s in Flatten([control.get('SECTION_START',[])])],
             [['-Map', str(e)] for e in target[1:]],
             '--start-group',
             [str(source[i]) for i in control['OBJSRCS']],
             '--end-group',
             '-o', str(target[0])]
      return ' '.join(Flatten(cmd))
   def qurt_program_emitter(target, source, env):
      if len(target) != 1:
         raise Exception("QurtProgram needs a single target")
      target_name = str(target[0])
      split_target = os.path.splitext(target_name)
      if not split_target[1]:
         target_name += '.elf'
      control = env['QPCONTROL']
      ntargets = [target_name]
      nsources = []
      specsrcs = []
      objsrcs = []
      cmdsrcs = []
      # If PRELINK is set in the instructions,
      #  set up some defaults
      if control.get('PRELINK', False):
         control.setdefault('MAPFILE', None)
         control.setdefault('AUTOLIB', False)
         control['LINKFLAGS'] = ['-r', '-d'] + control.get('LINKFLAGS', [])
      # Add map file to the target list
      #  Caller can disable map file creation by specifying MAPFILE=None
      if not 'MAPFILE' in control:
         # Caller didn't specify a map file name, use the target with a .map extension
         ntargets += [split_target[0]+'.map']
      else:
         mapfile = control['MAPFILE']
         if mapfile:
            # Caller specified a map file
            ntargets += [mapfile]
      psource = source
      if control.get('USER', False) and not control.get('PIE', False):
         # Caller requested to build a user mode ELF.
         smalldata = []
         adjenv = env
      else:
         # Caller requested to build a QuRT ELF.  Pass '-G 0'
         smalldata = ['-G', '0']
         # TBD - Add G0 to library paths just for this build
         adjenv = env.Clone(QDSP6_RELEASE_LIB_DIR = os.path.join(env['QDSP6_RELEASE_LIB_DIR'], 'G0'),
                            LIB_DSP_ROOT = os.path.join(env['LIB_DSP_ROOT'], 'G0'))
      if not 'AUTOLIB' in control or control['AUTOLIB']:
         # Caller didn't disable automatic libraries.  Add them here
         psource = Flatten([adjenv.subst(env['QDSP6OBJS1'])])
         psource += Flatten([adjenv.subst(env['QDSP6OBJS3'])])
         psource += source
         psource += Flatten([adjenv.subst(env['QDSP6OBJS4'])])
      else:
         psource = source
      if 'USER' in control and control['USER']:
         # Caller requested to build a user mode ELF.
         # Remove libqurtkernel
         psource = [x for x in psource if os.path.basename(str(x)) != 'libqurtkernel.a']
         # Change crt0 to crt1
         psource = [os.path.basename(str(x)) == 'crt0.o' and str(x).replace('crt0.o','crt1.o') or x for x in psource]
      control['LINK'] = env['LINK']
      if 'ISLAND' in control and control['ISLAND']:
         # Caller requested island mode.
         # Replace libqurtkernel with libqurtkernel_main & libqurtkernel_island
         findkernel = [j for j in range(len(psource)) if os.path.basename(str(psource[j])) == 'libqurtkernel.a']
         if findkernel:
            libkernel = str(psource.pop(findkernel[0]))
            psource.insert(findkernel[0], libkernel[:-2]+'_main.a')
            psource.insert(findkernel[0], libkernel[:-2]+'_island.a')
         control['LINK'] = ['python', '${Q6_RTOS_ROOT}/scripts/qurt-island-link.py', control['LINK']]
         nsources += ['${Q6_RTOS_ROOT}/scripts/qurt-island-link.py']
      # If libqurt is in the list after libc, move it before libc
      libc_search = [n for n in range(len(psource)) if os.path.basename(str(psource[n])) == 'libc.a']
      libqurt_search = [n for n in range(len(psource)) if os.path.basename(str(psource[n])) == 'libqurt.a']
      if libc_search and libqurt_search and libc_search[0] < libqurt_search[0]:
         libqurt = psource.pop(libqurt_search[0])
         psource.insert(libc_search[0], libqurt)
      if control.get('PIE', False):
         # User requested PIC processing
         # Add -pie to linker flags
         control['LINKFLAGS'] = control.get('LINKFLAGS', []) + ['-pie', '-fpic']
         control['SECTION_START'] = []
         newsources = []
         for src in psource:
            compo = os.path.split(env.GetBuildPath(src))
            maybe = os.path.join(compo[0], 'pic', compo[1])
            if maybe.endswith('.o'):
               maybe = maybe[:-2] + 'S.o'
            if os.path.isfile(maybe):
               newsources.append(maybe)
            elif 'INSTALL_DIR' in env and maybe.startswith(env['INSTALL_DIR']):
               newsources.append(maybe)
            else:
               newsources.append(src)
         psource = newsources
      for src in psource:
         srcname = str(src)
         ext = os.path.splitext(srcname)[1]
         if ext == '.c' or ext == '.cpp' or ext == '.s' or ext == '.S':
            # It's a C file that needs to be compiled
            objsrcs += [len(nsources)]
            nsources += [env.Object(src)]
         elif ext == '.xml' or ext == '.cfg':
            # It's a configuration file that needs to be built
            #  Assumes that cust_config.o is available as an object file name
            if 'ISLAND' in control and control['ISLAND']:
               objsrcs += [len(nsources)]
               nsources += [env.CustConfigObj('cust_config_main.o', src)]
               objsrcs += [len(nsources)]
               nsources += [env.CustConfigObj('cust_config_island.o', src)]
            else:
               objsrcs += [len(nsources)]
               nsources += [env.CustConfigObj('cust_config.o', src)]
         elif ext == '.cmd' or ext == '.lcs':
            # It's a linker command script
            cmdsrcs += [len(nsources)]
            nsources += [src]
         elif ext == '.ispec':
            # It's an island spec file
            specsrcs += [len(nsources)]
            nsources += [src]
         else:
            # It's not an extension recognized above; it's probably .o or .a; pass it directly to linker
            objsrcs += [len(nsources)]
            nsources += [src]
      if not control.get('PRELINK', False):
          if not ('USER' in control and control['USER']):
              #
              # It's not a prelink.  If we don't have an LCS file, add one
              #
              if not cmdsrcs:
                  cmdsrcs += [len(nsources)]
                  lcsargs = dict()
                  if control.get('ISLAND'):
                      lcsargs['island'] = True
                  if env.get('USE_SECURE_KERNEL'):
                      lcsargs['secure'] = True
                  nsources += [env.lcsbuilder(**lcsargs)]
                  new_start = Flatten([control.get('SECTION_START',[])])
                  new_start = [s.replace('.start','.qurtstart') for s in new_start]
                  control['SECTION_START'] = new_start
      control['OBJSRCS'] = objsrcs
      control['CMDSRCS'] = cmdsrcs
      control['SPECSRCS'] = specsrcs
      control['SMALLDATA'] = smalldata
      return ntargets, nsources
   def qurt_program_method(env, target, source, **varg):
      # target should be the ELF file to be generated
      # source should be a list of input files
      # varg is a dictionary containing the addition named arguments
      return env.Clone(QPCONTROL=varg).QurtProgramExt(target, source)
   env['BUILDERS']['QurtProgramExt'] = Builder(generator=qurt_program_generator, emitter=qurt_program_emitter)
   env.AddMethod(qurt_program_method, 'QurtProgram')

def lcs_generate (env):
    def LcsBuilder(env, target, source):
        import imp

        # Strip out any value nodes which are dictionaries, combine them, and pass them to create_lcs as keywords
        kw = dict()
        for N in source[:]:
            if isinstance(N,SCons.Node.Python.Value):
                V = N.read()
                if isinstance(V,dict):
                    source.remove(N)
                    kw.update(V)

        qurtisland = imp.load_source('qurtisland', str(source[0]))
        qurtisland.create_lcs(str(target[0]), str(source[1]), map(str,source[3:]), *map(str,source[2:3]), **kw)
    def LcsWrapper(env, target, source, **kw):
        #
        #  We just wrap the base version so that we can do two things:
        #   1.  Add the qurt-island-link.py to the sources.
        #   2.  Add default names for the input and output linker scripts.
        #
        #  One thing this allows to do is to just use env.lcsbuilder() with no arguments to get
        #   a default linker script.  If we just used an unwrapped Builder, that would be interpreted
        #   as a request to build 0 linker scripts from 0 input files and just return an empty list.
        #
        if not target:
            target = ['qurt_generated.lcs']
        if not source:
            source = ['$Q6_RTOS_ROOT/scripts/Input/qurt_default.lcs']
        if kw:
            source.insert(0, env.Value(kw))             # Any keyword args to lcs_builder get passed as a Value node
        source.insert(0, '$Q6_RTOS_ROOT/scripts/qurt-island-link.py')
        return env._LcsBuilder(target, source)
    env['BUILDERS']['_LcsBuilder'] = Builder(action = LcsBuilder)
    env['BUILDERS']['lcsbuilder'] = LcsWrapper

def qurt_object_scan_generate(env):
   qurt_object_scan_action = 'python $SOURCES $QURT_SCAN_FLAGS -T $Q6_TOOLS_ROOT -o $TARGET'

   def qurt_object_scan_emitter(target, source, env):
      # Emitter just adds the Python script at the *front* of the sources list.
      # This does triple duty of making the action command above get parsed
      #  correctly, getting the right path to the Python script, and creating
      #  a dependency on the Python script.
      return target, ['$INSTALL_SCRIPTS/qurt-object-scan.py']+source

   env['BUILDERS']['QurtObjectScan'] = Builder(action = qurt_object_scan_action,
                                               emitter = qurt_object_scan_emitter)

def qurt_filefromiterable_generate(env):
    def qurt_filefromstring_action(target, source, env):
        f = open(str(target[0]), 'wb')
        f.write(source[0].get_contents())
        f.close()
    def qurt_filefromiterable(env, target, iterable):
        return env._FileFromString(target, env.Value(''.join(map(str,iterable))))
    env['BUILDERS']['_FileFromString'] = Builder(action = Action(qurt_filefromstring_action, cmdstr='Building ${TARGET} ...'))
    env.AddMethod(qurt_filefromiterable, 'FileFromIterable')

    def qurt_filefromiterator_action(target, source, env):
        pfn, args = source[0].read()
        f = open(str(target[0]), 'wb')
        f.write(''.join(map(str,pfn(*args))))
        f.close()
    def qurt_filefromiterable_deferred(env, target, pfn, *args):
        return env._FileFromIterator(target, env.Value((pfn,args)))
    env['BUILDERS']['_FileFromIterator'] = Builder(action = Action(qurt_filefromiterator_action, cmdstr='Building ${TARGET} ...'))
    env.AddMethod(qurt_filefromiterable_deferred, 'FileFromIterableDeferred')

def test_lowfootprint_mem_analysis_generate(env):
    TestlowfootprintAnalysisBuilder = Builder(action = test_lowfootprint_analysis_builder)
    env['BUILDERS']['TestlowfootprintAnalysis'] = TestlowfootprintAnalysisBuilder
	
def test_lowfootprint_analysis_builder(target, source, env):
    import sys

    command = []
    command.append('python')
    command.append(env.subst('${Q6_RTOS_ROOT}/scripts/qurt_size_checker.py'))
    if 'FOOTPRINT_THRESHOLD' in env:
        command.append('--size')
        command.append(env.subst('${FOOTPRINT_THRESHOLD}K'))
    command.append(str(source[0]))

    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    if p.returncode:
        mesg = 'FAIL'
    else:
        mesg = 'PASS'

    sys.stderr.write(err)

    fo = open(str(target[0]), 'w')
    fo.write(mesg)
    fo.close()

    fo = open(str(target[1]), 'w')
    fo.write(out)
    fo.close()

    return p.returncode


#------------------------------------------------------------------------------
# Builder to change the DEFAULT_PHYSPOOL size
#------------------------------------------------------------------------------
def mem_budget_action(target, source, env):
    #
    #  Read the input file into an XML representation.
    #  Set the first region of DEFAULT_PHYSPOOL to $TEST_MEMORY_BUDGET.
    #  Write the output file from the modified XML representation.
    #
    import xml.dom.minidom
    doc = xml.dom.minidom.parse(str(source[0]))

    # Find all physical pool elements
    pools = doc.documentElement.getElementsByTagName('physical_pool')

    # Find DEFAULT_PHYSPOOL
    pools = [pool for pool in pools if pool.getAttribute('name')=='DEFAULT_PHYSPOOL']

    # Find regions of DEFAULT_PHYSPOOL
    regions = pools[0].getElementsByTagName('region')

    # Set the first region of DEFAULT_PHYSPOOL to have the right size
    regions[0].setAttribute('size', env.subst('$TEST_MEMORY_BUDGET'))

    # Write the modified document
    f = open(str(target[0]),'w')
    doc.writexml(f)
    f.write('\n')
    f.close()

def mem_budget_action_generate(env):
    MemBudgetXML1Builder = Builder(action = Action(mem_budget_action, cmdstr='Create memory patched ($TEST_MEMORY_BUDGET) XML $TARGET ...'),
                                   suffix = '.xml',
                                   src_suffix = '.xml')
    env['BUILDERS']['MemBudgetXML1'] = MemBudgetXML1Builder
                                     
