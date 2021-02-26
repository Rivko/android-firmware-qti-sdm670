# ============================================================================
#
# Mink Audit Builder build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to process and audit Mink interface and syscall code to
#    detect and report changes, failing the build if necessary.
#
# Copyright (c) 2016 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
# ----------------------------------------------------------------------------
import sys
import os

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

def exists(env):
   return 1

def generate(env):
  env.AddMethod(run_idl_audit,'RunIDLAudit')
  env.AddMethod(run_syscall_audit,'RunSyscallAudit')
  bld = env.Builder(action=cmnlib_audit_builder)
  env.Append(BUILDERS={'CmnlibAuditBuilder':bld})
  

def run_idl_audit(env):
  sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
  from mink_audit_cfgrw import IDLAudit
  idl_auditor = IDLAudit(os.path.join(THIS_DIR,'idl_audit.cfg'))
  ret = idl_auditor.audit()
  if ret == 0:
    print "IDL audit succeeded"
  else:
    print "IDL audit failed"
    print 'please see trustzone_images/ssg/bsp/build/scripts/audit_scripts_howto.txt for more info'
    env.Exit(1)

def run_syscall_audit(env):
  sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
  from mink_audit_cfgrw import SyscallAudit
  syscall_auditor = SyscallAudit(os.path.join(THIS_DIR,'syscall_audit.cfg'))
  ret = syscall_auditor.audit()
  if ret == 0:
    print "syscall audit succeeded"
  else:
    print "syscall audit failed"
    print 'please see trustzone_images/ssg/bsp/build/scripts/audit_scripts_howto.txt for more info'
    env.Exit(1)
    
def cmnlib_audit_builder(env, target, source):
  sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
  
  cmnlib_so_path = str(source[0])
  from mink_audit_cfgrw import CommonlibAudit
  cmnlib_auditor = CommonlibAudit(cmnlib_so_path, env.RealPath('${LLVMBIN}/llvm-readelf'))
  ret = cmnlib_auditor.audit()
  if ret == 0:
    print "cmnlib audit succeeded"
    with open(str(target[0]), 'w') as f:
      f.write('cmnlib audit succeeded')
  else:
    with open(str(target[0]), 'w') as f:
      f.write('cmnlib audit failed')
    print "cmnlib audit failed"
    print 'please see trustzone_images/ssg/bsp/build/scripts/audit_scripts_howto.txt for more info'
    env.Exit(1)
  
  
  