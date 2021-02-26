#! /usr/bin/env python
#
#  Build the QuRT Secure Resource Manager
#
#  Usage:
#    python /path/to/qurt-srm-build.py [options] object_files
#
#  Options:
#    --output-file OUTPUT_FILE
#    --start-address VIRTUAL_ADDRESS
#    --hexagon-tools TOOLS_BASE_PATH
#    --qurt-location QURT_LOCATION
#    --lcs-file LCSFILENAME
#    --map-file MAPFILENAME
#
#  Each of these has a default:
#    The default output file is qurt_SRM.elf in the current directory.
#    The default start address is 0xFD000000.  (But this may change in the future.)
#    The default hexagon tools path is determined by searching for hexagon-clang on the current OS path.
#    The default QuRT location is determined by using the QuRT tree in which this script is found.
#    The default LCS filename is the output file name with .lcs appended.
#    The default MAP filename is the output file name with .map appended.
#

import optparse
import shlex
import textwrap
import distutils.spawn
import os.path
from lib.qurt import *

class SRMBuild(object):
    compiler_name = 'hexagon-clang'
    linker_name = 'hexagon-link'

    def __init__(self):
        pass
    def parse_args(self, argv):
        parser = optparse.OptionParser()

        parser.add_option('-o', '--output-file',
                          default='qurt_SRM.elf',
                          help='specify the destination ELF file',
                          metavar='FILE')
        parser.add_option('-a', '--start-address',
                          default='0x74000000',
                          help='specify the base virtual address',
                          metavar='ADDR')
        parser.add_option('-z', '--virtual-size',
                          default='0x04000000',
                          help='specify the size of the virtual address range',
                          metavar='SIZE')
        parser.add_option('-T', '--hexagon-tools',
                          help='specify the root path for Hexagon tools',
                          metavar='TOOLS')
        parser.add_option('-Q', '--qurt-location',
                          help='specify the root path for the QuRT installation',
                          metavar='QURT')
        parser.add_option('-L', '--lcs-file',
                          help='specify the name for the temporary LCS file',
                          metavar='LCSFILE')
        parser.add_option('-M', '--map-file',
                          help='specify the name for the linker map file',
                          metavar='MAPFILE')

        #
        #  Do @fromfile replacement.  For each input arg which starts with the @ sign,
        #   read the contents of the referenced file and insert its parsed contents.
        #

        i = 1
        while i < len(argv):
            arg = argv[i]
            if arg[0] == '@':
                try:
                    f = open(arg[1:],'rU')
                    tmp = f.read()
                    f.close()
                except IOError:
                    raise Exception('Error reading contents of %s' % arg[1:])
                argv[i:i+1] = shlex.split(tmp)
            else:
                i += 1

        (opts, args) = parser.parse_args(argv[1:])

        self.output_file = os.path.realpath(opts.output_file)

        if opts.lcs_file == None:
            self.linker_script = self.output_file + '.lcs'
        else:
            self.linker_script = os.path.realpath(opts.lcs_file)

        if opts.map_file == None:
            self.map_file = self.output_file + '.map'
        else:
            self.map_file = os.path.realpath(opts.map_file)

        self.hexagon_tools = opts.hexagon_tools
        if self.hexagon_tools == None:
            #
            # Find hexagon-clang on the path, then go to ../.. from
            #  its location.
            #
            tmp = distutils.spawn.find_executable(self.compiler_name)
            if tmp == None:
                raise Exception('Cannot find %s on the PATH' % self.compiler_name)
            tmp = os.path.dirname(tmp)
            tmp = os.path.join(tmp, '..', '..')
            tmp = os.path.normpath(tmp)
            self.hexagon_tools = tmp

        self.qurt_location = opts.qurt_location
        if self.qurt_location == None:
            #
            # Find this script, then go to .. from its location.
            #
            tmp = os.path.realpath(__file__)
            tmp = os.path.dirname(tmp)
            tmp = os.path.join(tmp, '..')
            tmp = os.path.normpath(tmp)
            self.qurt_location = tmp

        try:
            #
            # Convert start address to integer, make sure it's a 4K aligned 32-bit number
            #
            self.start_address = int(opts.start_address, 0)
            if not (0 <= self.start_address <= 0xFFFFFFFF):
                raise ValueError()
            if (self.start_address & 0xFFF) != 0:
                raise ValueError()
        except ValueError:
            raise Exception('Start address value not valid: %s' % opts.start_address)

        try:
            #
            # Convert virtual size to integer, make sure it combines with start address
            #  to not overflow 32 bits, and that it's 4K aligned
            #
            self.virtual_size = int(opts.virtual_size, 0)
            if not (0 <= self.virtual_size <= 0xFFFFFFFF):
                raise ValueError()
            if (self.virtual_size & 0xFFF) != 0:
                raise ValueError()
            if not (0 <= self.start_address + self.virtual_size <= 0xFFFFFFFF):
                raise ValueError()
        except ValueError:
            raise Exception('Virtual size value not valid: %s' % opts.virtual_size)

        self.args = args

        print 'Running %s with options:' % __file__
        print '  Output file:     %s' % self.output_file
        print '  LCS file:        %s' % self.linker_script
        print '  Map file:        %s' % self.map_file
        print '  Tools path:      %s' % self.hexagon_tools
        print '  QuRT path:       %s' % self.qurt_location
        print '  Start address:   0x%08X' % self.start_address
        print '  Virtual size:    0x%08X' % self.virtual_size
        print 'Input files are: %s' % args

    def get_info(self):
        #
        #  Read some needed information.  In particular, this
        #   reads $QURT_LOCATION/scripts/Input/build_params.txt
        #   to find the Q6VERSION to make the compiler argument
        #   such as -mv60, -mv62, etc.
        #
        paramsfile = os.path.join(self.qurt_location, 'scripts', 'Input', 'build_params.txt')
        try:
            f = open(paramsfile, 'rU')
            params = f.read()
            f.close()
        except IOError:
            raise Exception('Problem reading QuRT build params: %s' % paramsfile)
        for s in shlex.split(params):
            tmp = s.split('=')
            if len(tmp) == 2 and tmp[0] == 'Q6VERSION':
                self.architecture = tmp[1]
                self.machine_arg = '-m'+tmp[1]
                break
        else:
            raise Exception('Cannot determine Q6VERSION')

    def make_lcs(self):
        #
        #  Build a linker script template in the same directory as the
        #   output file.
        #
        #  For SRM purposes, we want a hard boundary between the
        #   read-only portion and the read-write portion, so we
        #   organize the order of sections like this:
        #    1) read-only data
        #    2) read-execute text
        #    3) read-write data
        #    4) read-write ZI space
        #
        #  By doing this, we basically force the ELF file into a
        #   format where we can't accidentally leave read-only
        #   data with write permission enabled.
        #
        lcs = '''
            SECTIONS {
                . = 0x%X;
                .srm.rodata.user.main.DECL: {
                    . = ALIGN(4);
                    qurt_srm_driver_start = .;
                    KEEP(*(.srm.rodata.user.main.DECL))
                    qurt_srm_driver_end = .;
                }
                .eh_frame_hdr.srm.main: {
                    *(.eh_frame_hdr)
                }
                .gcc_except_table.srm.main: {
                    *(.gcc_except_table .gcc_except_table.*)
                }
                .eh_frame.srm.main: {
                    KEEP(*(.eh_frame))
                }
                .rodata.srm.main: {
                    *(.rodata .rodata.* .gnu.linkonce.r.*)
                }
                .ctors.srm.main: ONLY_IF_RO {
                    KEEP(%s(.ctors))
                    KEEP(*(EXCLUDE_FILE(%s) .ctors))
                    KEEP(*(SORT(.ctors.*)))
                    KEEP(*(.ctors))
                }
                .dtors.srm.main: ONLY_IF_RO {
                    KEEP(%s(.dtors))
                    KEEP(*(EXCLUDE_FILE(%s) .dtors))
                    KEEP(*(SORT(.dtors.*)))
                    KEEP(*(.dtors))
                }
                .start: {
                    KEEP(*(.start))
                }
                .text.srm.main: {
                    *(.text .stub .text.* .gnu.linkonce.t.*)
                }
                .init.srm.main: {
                    KEEP(*(.init))
                }
                .fini.srm.main: {
                    KEEP(*(.fini))
                }
                . = ALIGN(4K);
                .data.srm.main: {
                    *(.data .data.* .gnu.linkonce.d.*)
                }
                .ctors.srm.main: ONLY_IF_RW {
                    KEEP(%s(.ctors))
                    KEEP(*(EXCLUDE_FILE(%s) .ctors))
                    KEEP(*(SORT(.ctors.*)))
                    KEEP(*(.ctors))
                }
                .dtors.srm.main: ONLY_IF_RW {
                    KEEP(%s(.dtors))
                    KEEP(*(EXCLUDE_FILE(%s) .dtors))
                    KEEP(*(SORT(.dtors.*)))
                    KEEP(*(.dtors))
                }
                . = ALIGN(128);
                _SDA_BASE_ = .;
                .sdata.srm.main: {
                    *(.lit* .gnu.linkonce.l*)
                    *(.sdata .sdata.* .gnu.linkonce.s.*)
                    /* Next lines are a hack to make sure that the SDATA section is at least 16 bytes long
                       so that the ELF signing process doesn't complain about a small load segment */
                    . = .+16;
                    __sdata_endloc = .;
                }
                .sbss.srm.main: {
                    __sbss_start = .;
                    *(.dynsbss)
                    *(.sbss .sbss.* .gnu.linkonce.sb.*)
                    *(.scommon .scommon.*)
                    __sbss_end = .;
                }
                .bss.srm.main: {
                    __bss_start = .;
                    *(.dynbss)
                    *(.bss .bss.* .gnu.linkonce.b.*)
                    *(COMMON)
                    _end = .;
                }
                . = ALIGN(4K);
                .shadow.srm.build.VSPACE: {
                    __srm_vspace_start = .;
                    . = 0x%X;
                    __srm_vspace_end = .;
                }
                .comment: {
                    *(.comment)
                }
                .debug_info: { *(.debug_info) }
                .debug_abbrev: { *(.debug_abbrev) }
                .debug_ranges: { *(.debug_ranges) }
                .debug_aranges: { *(.debug_aranges) }
                .debug_line: { *(.debug_line) }
                .debug_str: { *(.debug_str) }
                .debug_macinfo: { *(.debug_macinfo) }
                .debug_frame: { *(.debug_frame) }
                .debug_loc: { *(.debug_loc) }
                .debug_pubnames: { *(.debug_pubnames) }
                .debug_pubtypes: { *(.debug_pubtypes) }
                . = ALIGN(4K);
                .shadow.srm.build.UNRECOGNIZED: {
                    *(*)
                }
            }
        ''' % (self.start_address,
               self.init_obj,
               self.fini_obj,
               self.init_obj,
               self.fini_obj,
               self.init_obj,
               self.fini_obj,
               self.init_obj,
               self.fini_obj,
               self.start_address + self.virtual_size)

        lcs = textwrap.dedent(lcs)

        f = open(self.linker_script, 'w')
        f.write(lcs)
        f.close()

    def main(self, argv):
        self.parse_args(argv)
        self.get_info()

        #
        # TBD -- scan the objects and create the SRM database
        #

        self.init_obj = os.path.join(self.hexagon_tools, 'Tools', 'target', 'hexagon', 'lib', self.architecture, 'G0', 'init.o')
        self.fini_obj = os.path.join(self.hexagon_tools, 'Tools', 'target', 'hexagon', 'lib', self.architecture, 'G0', 'fini.o')

        self.make_lcs()

        cmd = [
            os.path.join(self.hexagon_tools, 'Tools', 'bin', self.linker_name),
            self.machine_arg,
            '-o', self.output_file,
            '-g',
            '-G0',
            '-M',
            '-Map', self.map_file,
            '-T', self.linker_script,
            '--gc-sections',
            '-nostdlib',
            '--start-group',
            os.path.join(self.qurt_location, 'lib', 'crt1_srm.o'),
            self.init_obj,
            os.path.join(self.qurt_location, 'lib', 'srm_program.o'),
            self.args,
            os.path.join(self.hexagon_tools, 'Tools', 'target', 'hexagon', 'lib', self.architecture, 'G0', 'libc.a'),
            os.path.join(self.hexagon_tools, 'Tools', 'target', 'hexagon', 'lib', self.architecture, 'G0', 'libqcc.a'),
            self.fini_obj,
            '--end-group']

        run_command(cmd)

        return 0

if __name__ == '__main__':
    #
    #  Being executed from the command line
    #
    run_script(SRMBuild().main)

# Signatures of the files that this depends on
# 46a31eb420b1999d63d43a1df42e94ca /local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/core/kernel/qurt/build/sensor_img/qdsp6/670.adsp.prod/install/ADSPv65MP/scripts/lib/qurt.py
