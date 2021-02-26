#!/usr/bin/env python

# pack_elf.py
#
# Remove sections and symbols from elf files
#
# Usage:
#    python pack_elf.py
#
#

import os
import platform
import subprocess
import sys

# Rules for removing symbols from elf files
ELF_SYMBOL = {
   'qsee': {  # arbitrary rule name.  Must be unique within the ELF_SYMBOL list.

      # All symbols will be stripped from the elf file
      # Use the 'keep' list to specify symbols that should remain.
      # Use '*' for wildcard matching.
      'keep':  [
         '*psci*',
         'g_tzbsp_diag',
         'thread_list',
         'SysdbgLogBuffer',
      ],

      # If you use a wildcard in the 'keep' list and need to remove some additional symbols,
      # add them to the 'strip' list.

      #'strip':  ['psci_cpu*'],

      # Remove execution 'X' code sections
      'remove-sections': [
         'TZBSP_ENTRY_ER',
         'EL1*',
         'EL0*',
         'TZ_PIMEM_CODE_ER',
      ],

      # Input elf file name
      'input': './ssg/bsp/qsee/build/WAXAANAA/qsee.elf',

      # Output elf file name
      'output': 'pack_elf/qsee.elf'
   },

   'hyp': {
      'keep':  [
         'hyp_smc_log',
         'pt_context',
         'g_pHyp_log',
      ],

      'remove-sections': [
         'HYP_CODE1',
         'EL2*',
         'LOW*',
         'CMMU_ASM',
      ],

      'input': './core/bsp/hypervisor/build/WAXAANAA/hyp.elf',
      'output': 'pack_elf/hyp.elf'
   },

   'mon': {
      'keep':  [
         'mon_warm_entry',
       ],

      'remove-sections': [
         'EL3*',
         'ARMV*',
         'SYSINI',
         '.rodata*',
         'MON_IMEM_UNCACHED*',
         'LEL*',
         'TEMP',
         'MON_PIMEM*',
         'MON_EL1*',
         'MON_L1*',
      ],

      'input': './ssg/bsp/monitor/build/WAXAANAA/mon.elf',
      'output': 'pack_elf/mon.elf'
   }
}


def strip_elf(elf, output = None):
   """Remove the symbols from the given 'elif' & create 'putput <elf>_stripped.elf"""
   if output == None and not os.path.isdir('./pack_elf'):
      os.mkdir('./pack_elf')

   elf_name, elf_ext = os.path.splitext(os.path.basename(elf))

   args = [ get_strip_tool(), '--strip-all' ]

   elf_info = ELF_SYMBOL.get(elf_name)
   assert elf_info != None, "BUG: No symbol rules found for {}".format(elf)

   # Keep symbols
   print 'Processing rule: ' + elf
   for symbol in elf_info.get('keep', []):
      print '   keep symbol ' + symbol
      args.append('--keep-symbol="' + symbol + '"')

   # Additional symbols to remove
   for symbol in elf_info.get('strip', []):
      print '   strip symbol ' + symbol
      args.append('--strip-symbol="' + symbol + '"')

   # Remove elf sections
   # Todo: Run readelf --sections and remove 'X' executable sections automatically
   for symbol in elf_info.get('remove-sections', []):
      print '   remove section ' + symbol
      args.append('--remove-section="' + symbol + '"')

   # specify architecture
   args.extend([
      '-w',
      '--target=elf64-little',
   ])

   # specify output file
   if output == None:
      output = elf_info['output']

   args.extend([
      '-o',
      output,
   ])

   # specify input file
   if elf_ext == ".elf":
      args.append(elf)
   else:
      args.append(elf_info['input'])

   args = " ".join(args)
   print '\nRunning command: '
   print args

   # strip only produces a correct elf with symbols when shell=True ...
   output = subprocess.check_output(args, shell=True)
   print output


def get_strip_tool():
   if "win" in platform.system().lower():
      gnuroot = os.path.join(os.environ["GNUROOT"], "bin")
      strip_tool = "aarch64-linux-gnu-strip.exe"
   else:
      gnuroot = "/pkg/qct/software/arm/linaro-toolchain/aarch64-none-elf/4.9-2014.07/bin"
      strip_tool = "aarch64-none-elf-strip"

   return os.path.join(gnuroot, strip_tool)

def run():
   """Simple command-line handler"""
   assert len(sys.argv) in (1, 3), "Invalid arguments (requires 0 or 2)"
   if len(sys.argv) == 3:
      elfs = [ sys.argv[1] ]
      output = sys.argv[2]
   else: # len(sys.argv) == 1
      elfs = [ key + ".elf" for key in ELF_SYMBOL.keys() ]
      output = None

   for elf in elfs:
      strip_elf(elf, output)

if __name__ == "__main__":
   run()
