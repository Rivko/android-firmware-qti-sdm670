# ----------------------------------------------------------------------------
# Copyright (c) 2016, 2017 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ----------------------------------------------------------------------------

import fnmatch
import os
import re
import pickle
import subprocess
import posixpath
from hashlib import sha256

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..','..','..'))
SYSCALL_HANDLER_FILE = os.path.join(ROOT_DIR,'ssg','securemsm','trustzone','qsee','mink','app_mgr','src','syscall_handler.c')

def hash_file(path_to_file):
  with open(path_to_file) as f:
    return sha256(f.read()).hexdigest()

class IDLAudit():
  #the idl audit is stored as posix style relative path <> hash in idl_audit.cfg
  #but it's converted to absolute path <> hash to get around windows/posix path differences
  def __init__(self, audit_cfg, regenerating_audit=False):
    self.audit_cfg_file = audit_cfg
    if not regenerating_audit:
      with open(audit_cfg, "r") as f:
        self.idl_to_sha_map = dict(pickle.load(f))
      
  def write_out_audit_cfg(self, audit):
    with open(self.audit_cfg_file, "w") as f:
      pickle.dump(sorted(audit.items()), f)
  
  def find_all_idls(self):
    all_idls = []
    for root, dirs, files in os.walk(ROOT_DIR):
      for file in files:
        if file.endswith('.idl'):
          all_idls.append(os.path.relpath(os.path.join(root,file), os.path.dirname(__file__)))
    all_idls = [s.replace('\\','/') for s in all_idls]
    return all_idls
    
  def audit(self):
    ret = 0
    existing_idls = self.find_all_idls()
    added_files = set(existing_idls) - set(self.idl_to_sha_map.keys())
    if len(added_files) > 0:
      ret = 1
      print "the following .idl files have been added"
      print "please add them to the idl audit"
      for f in sorted(added_files):
        print '\t',f
    
    deleted_files = set(self.idl_to_sha_map.keys()) - set(existing_idls)
    if len(deleted_files) > 0:
      ret = 1
      print "the following .idl files have been deleted"
      print "please remove them to the idl audit"
      for f in sorted(deleted_files):
        print '\t',f
    
    idl_mismatches = set()
    for idl_file in sorted(set(existing_idls).intersection(self.idl_to_sha_map.keys())):
      if hash_file(idl_file) != self.idl_to_sha_map[idl_file]:
        idl_mismatches.add(idl_file)
    if len(idl_mismatches) > 0:
      ret = 1
      print "the following idl files don't match the audit"
      print "please update the mink audit"
      for f in sorted(idl_mismatches):
        print '\t',f
    return ret
    
  def regenerate_audit(self):
    new_audit = dict()
    all_idls = self.find_all_idls()
    for idl in all_idls:
      new_audit[idl] = hash_file(idl)
    self.idl_to_sha_map = new_audit
    self.write_out_audit_cfg(new_audit)

    
class SyscallAudit():

  def __init__(self, audit_cfg, regenerating_audit=False):
    self.audit_cfg_file = audit_cfg
    if not regenerating_audit:
      with open(audit_cfg, "r") as f:
        self.syscall_set = set(pickle.load(f))
      
  def write_out_audit_cfg(self, audit):
    with open(self.audit_cfg_file, "w") as f:
      pickle.dump(sorted(audit), f)
      
  def find_syscalls(self):
    all_syscalls = set()
    matcher = re.compile('\s+case\s+SYSNUM\(([a-zA-Z_][a-zA-Z_0-9]*)\)')
    with open(SYSCALL_HANDLER_FILE,'r') as f:
      for line in f:
        match = matcher.search(line)
        if match is not None:
          all_syscalls.add(match.group(1))
    return all_syscalls
    
  def audit(self):
    ret = 0
    all_syscalls = self.find_syscalls()
    removed = self.syscall_set - all_syscalls
    added = all_syscalls - self.syscall_set
    if len(removed) > 0:
      print "syscalls have been removed from {}".format(SYSCALL_HANDLER_FILE)
      print "do not touch this file"
      print removed
      ret = 1
    if len(added) > 0:
      print "syscalls have been added to {}".format(SYSCALL_HANDLER_FILE)
      print "do not touch this file"
      print added
      ret = 1
    return ret
      
  def regenerate_audit(self):
    self.syscall_set = self.find_syscalls()
    self.write_out_audit_cfg(self.syscall_set)

    
class CommonlibAudit():
  
  def __init__(self, cmnlib_so_path, readelf_path, regenerating_audit=False):
    self.cmnlib_so_path = cmnlib_so_path
    self.readelf_path = readelf_path
    if cmnlib_so_path.endswith('libcmnlib.so'):
      self.audit_cfg_file = os.path.abspath(os.path.join(THIS_DIR,'cmnlib32_audit.cfg'))
    else:
      self.audit_cfg_file = os.path.abspath(os.path.join(THIS_DIR,'cmnlib64_audit.cfg'))
    if not regenerating_audit:
      with open(self.audit_cfg_file, 'r') as f:
        self.symbols_set = set(pickle.load(f))
    
  def write_out_audit_cfg(self, audit):
    with open(self.audit_cfg_file, "w") as f:
      pickle.dump(sorted(audit), f)
      
  def find_symbols(self):
    all_symbols = set()
    output = subprocess.check_output([self.readelf_path, '-s', self.cmnlib_so_path])
    for line in output.split('\n'):
      line = line.strip()
      # we only care about symbols in the .dynsym section
      if re.match("^Symbol table",line):
        if line.split()[2] != "'.dynsym'":
          return all_symbols
      # all lines we care about look like this
      # Num:    Value  Size Type    Bind   Vis      Ndx Name
      # 30: 0002177b    48 FUNC    GLOBAL DEFAULT    1 qsee_strlcpy
      if not re.match("^[0-9]+:", line):
        continue
      #Things where Ndx is UND are weird and not really commonlib exports but
      #still show up in dynsym, so ignore them
      if line.split()[6] == 'UND':
        continue
        
      #we only audit functions, not other stuff that the linker and/or compiler
      #may or may not have thrown in like NOTYPE
      if line.split()[3] != "FUNC":
        continue

      #symbol name is always the last word in the line
      # since we're only looking in .dynsym we can assume everything is global and default visibility
      line = line.split()[-1]
      #sometimes readelf appends '@' to the symbol name for whatever reason, so strip that off
      if line.endswith('@'):
        line = line[:-1]
      # with this logic we sometimes see lines that are length 0 (because of the @ stuff)
      # so if the symbol name is 0 characters long, just skip
      if len(line) > 0:
        all_symbols.add(line)
    return all_symbols
    
  def audit(self):
    ret = 0
    all_symbols = self.find_symbols()
    removed = self.symbols_set - all_symbols
    added = all_symbols - self.symbols_set
    if len(removed) > 0:
      print "Please do not modify entry points to commonlib"
      print "The following symbols have been removed from commonlib"
      print sorted(removed)
      ret = 1
    if len(added) > 0:
      print "Please do not modify entry points to commonlib"
      print "The following symbols have been added to commonlib"
      print sorted(added)
      ret = 1
    return ret

  def regenerate_audit(self):
    sym_set = self.find_symbols()
    self.write_out_audit_cfg(sym_set)

