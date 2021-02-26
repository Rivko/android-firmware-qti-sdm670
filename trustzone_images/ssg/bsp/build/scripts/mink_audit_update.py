# ----------------------------------------------------------------------------
# Copyright (c) 2016 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ----------------------------------------------------------------------------

from mink_audit_cfgrw import IDLAudit, SyscallAudit, CommonlibAudit
import os
import argparse

'''
See audit_scripts_howto.txt to see how to update the audits
'''

parser = argparse.ArgumentParser(description='Compare hashes of critical files to mark modifications')
parser.add_argument('-c', '--config', dest="config_file_name",
                    help='''Path of config file containing the audit you want to update.
Acceptable values are 'cmnlib32_audit.cfg', 'cmnlib64_audit.cfg', 'idl_audit.cfg', 'syscall_audit.cfg' ''', default=None,
                    metavar="FILE")
parser.add_argument('--readelf_path', dest='readelf_path',
                    help='full path to the readelf executable', default=None, metavar="FILE")
parser.add_argument('--commonlib_path', dest='commonlib_path',
                    help='full path to libcmnlib.so or libcmnlib64.so', default=None, metavar="FILE")



args = parser.parse_args()

if args.config_file_name == 'cmnlib32_audit.cfg':
  cmnlib32_audit = CommonlibAudit(args.commonlib_path, args.readelf_path, regenerating_audit=True)
  cmnlib32_audit.regenerate_audit()
elif args.config_file_name == 'cmnlib64_audit.cfg':
  cmnlib64_audit = CommonlibAudit(args.commonlib_path, args.readelf_path, regenerating_audit=True)
  cmnlib64_audit.regenerate_audit()
elif args.config_file_name == 'idl_audit.cfg':
  idl_auditor = IDLAudit('idl_audit.cfg', regenerating_audit=True)
  idl_auditor.regenerate_audit()
elif args.config_file_name == 'syscall_audit.cfg':
  syscall_auditor = SyscallAudit('syscall_audit.cfg', regenerating_audit=True)
  syscall_auditor.regenerate_audit()
