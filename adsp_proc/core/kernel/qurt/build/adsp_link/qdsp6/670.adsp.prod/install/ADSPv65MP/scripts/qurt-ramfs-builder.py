#! /usr/bin/python
#=============================================================================
#
#                                    qurt-ramfs-builder.py
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

import struct

from lib.qurt import *

class QurtRamfsBuilder:
    def parse_args(self,argv):
        from optparse import OptionParser, OptionGroup
        from os.path import basename

        parser = OptionParser(usage='%prog [parameters] [[flags...] input_file]...')

        def add_flag(parser, key):
            if len(parser.largs) == 0 or not isinstance(parser.largs[-1],list):
                parser.largs.append(list())
            parser.largs[-1].append(key)

        def opt_callback_align(option, opt_str, value, parser):
            try:
                value = convert_size(value)
            except ValueError:
                parser.error('Invalid format for --align')
            add_flag(parser, 'align')
            add_flag(parser, value)
            if 'align' in parser.largs[-1][:-2:2]:
                parser.error('Only one align flag can be specified per file')

        group = OptionGroup(parser, 'Required Parameters')
        group.add_option('-o', action='store', dest='outfilename',
                         help='write output to FILENAME', metavar='FILENAME')
        group.add_option('-p', action='store', dest='physaddr',
                         help='set base physical address of output file to PHYSADDR', metavar='PHYSADDR')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Flags affecting the following input file')
        group.add_option('--align', action='callback', callback=opt_callback_align, type='string',
                         help='mark the next input file as being aligned to BOUNDARY in physical memory', metavar='BOUNDARY')
        parser.add_option_group(group)

        (opts, args) = parser.parse_args()

        self.outfilename = opts.outfilename
        if self.outfilename == None:
            parser.error('Must provide an output file name using -o')

        try:
            if opts.physaddr == None:
                raise ValueError()
            self.physaddr = convert_size(opts.physaddr)
            if (self.physaddr & 0xFFFFF000) != self.physaddr:
                raise ValueError()
        except ValueError:
            parser.error('Must provide a valid 4K-aligned physical address using -p')

        if not args:
            parser.error('Cannot create an empty filesystem')

        if isinstance(args[-1],list):
            parser.error('Flags found with no file name following')

        self.args = []
        flags = ()
        for x in args:
            if isinstance(x,list):
                flags = tuple(x)
            else:
                if '=' in x:
                    runtime_file_name, build_file_name = x.split('=',1)
                else:
                    runtime_file_name, build_file_name = (basename(x), x)
                self.args.append([flags,runtime_file_name,build_file_name])
                flags = ()

    def main(self, argv):
        self.parse_args(argv)

        print 'Output file name is %s' % self.outfilename
        print 'Output address is 0x%X' % self.physaddr

        outfile = open(self.outfilename, 'w+b')
        cur_addr = self.physaddr

        # Skip ahead 4K in the output file.
        # We will seek back and rewrite it when we're done.

        outfile.write('\0' * 4096)

        namestrs = ''
        filecount = 0

        for arg in self.args:
            flags = arg[0]
            runtime_file_name = arg[1]
            build_file_name = arg[2]
            align = 0x1000
            for k,v in zip(flags[::2],flags[1::2]):
                if k == 'align':
                    align = v
            saddr = cur_addr
            saddr += align-1
            saddr -= saddr % align
            outfile.write('\0' * (saddr-cur_addr))
            cur_addr = saddr
            arg.append(cur_addr - self.physaddr)   # arg[3] gets offset
            infile = open(build_file_name, 'rb')
            sz = 0
            while True:
                buf = infile.read(0x40000)         # 256K at a time
                if buf == '':
                    break
                outfile.write(buf)
                cur_addr += len(buf)
                sz += len(buf)
            arg.append(sz)                         # arg[4] gets size
            arg.append(len(namestrs))              # arg[5] gets name offset
            namestrs += runtime_file_name+'\0'
            filecount += 1

        nameadjust = (3+filecount)*16

        info_addr = cur_addr
        info_addr += 0xFFF
        info_addr -= info_addr % 0x1000

        outfile.write('\0' * (info_addr-cur_addr))

        # Go back and read the first 16 bytes from the file

        tmp = outfile.tell()
        outfile.flush()
        outfile.seek(4096, 0)
        startdata = outfile.read(16)
        outfile.flush()
        outfile.seek(tmp, 0)
        outfile.write(startdata)

        finalsizeloc = outfile.tell()+8
        outfile.write(struct.pack('<LLLL', 0xFFFFFFFF, 0, 0, 0))
        for arg in self.args:
            outfile.write(struct.pack('<LLLL', arg[5]+nameadjust, arg[3], arg[4], 1))
        outfile.write(struct.pack('<LLLL', 0, 0, 0, 0))
        outfile.write(namestrs)

        # Pad out to 4K boundary
        sz = outfile.tell() & 0xFFF
        if sz:
            outfile.write('\0' * (4096-sz))

        totalsize = outfile.tell() - 4096

        info_offset = (info_addr - self.physaddr) / 4096

        outfile.flush()
        outfile.seek(finalsizeloc, 0)
        outfile.write(struct.pack('<L', totalsize))

        outfile.flush()
        outfile.seek(4096, 0)
        outfile.write(struct.pack('<LLLL', 0x31534651, 0x3CC36996, info_offset, 0x55555555))

        outfile.flush()
        outfile.seek(0, 0)

        # Write an ELF header, a program header, a section header, and a string table

        strtab = '\0.shstrtab\0.rodata\0'

        ehdr = Elf32Header(None,
                           e_ident = '\x7fELF\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00',
                           e_type = 2,
                           e_machine = 164,
                           e_version = 1,
                           e_entry = self.physaddr,
                           e_phoff = 52,
                           e_shoff = 84,
                           e_flags = 4,
                           e_ehsize = 52,
                           e_phentsize = 32,
                           e_phnum = 1,
                           e_shentsize = 40,
                           e_shnum = 3,
                           e_shstrndx = 1)

        phdr = Elf32ProgramHeader(None,
                                  p_type = Elf32ProgramHeader.PT_LOAD,
                                  p_offset = 4096,
                                  p_vaddr = self.physaddr,
                                  p_paddr = self.physaddr,
                                  p_filesz = totalsize,
                                  p_memsz = totalsize,
                                  p_flags = 4,
                                  p_align = 4096)

        shdr0 = Elf32SectionHeader(None,
                                   sh_name = 0,
                                   sh_type = 0,
                                   sh_flags = 0,
                                   sh_addr = 0,
                                   sh_offset = 0,
                                   sh_size = 0,
                                   sh_link = 0,
                                   sh_info = 0,
                                   sh_addralign = 0,
                                   sh_entsize = 0)

        shdr1 = Elf32SectionHeader(None,
                                   sh_name = strtab.index('.shstrtab\0'),
                                   sh_type = Elf32SectionHeader.SHT_STRTAB,
                                   sh_flags = 0,
                                   sh_addr = 0,
                                   sh_offset = 52+32+40+40+40,
                                   sh_size = len(strtab),
                                   sh_link = 0,
                                   sh_info = 0,
                                   sh_addralign = 0,
                                   sh_entsize = 0)

        shdr2 = Elf32SectionHeader(None,
                                   sh_name = strtab.index('.rodata\0'),
                                   sh_type = Elf32SectionHeader.SHT_PROGBITS,
                                   sh_flags = Elf32SectionHeader.SHF_ALLOC,
                                   sh_addr = self.physaddr,
                                   sh_offset = 4096,
                                   sh_size = totalsize,
                                   sh_link = 0,
                                   sh_info = 0,
                                   sh_addralign = 4096,
                                   sh_entsize = 0)

        outfile.write(ehdr.output())
        outfile.write(phdr.output())
        outfile.write(shdr0.output())
        outfile.write(shdr1.output())
        outfile.write(shdr2.output())
        outfile.write(strtab)
        outfile.flush()
        outfile.close()

        return 0

if __name__ == '__main__':
    from lib.qurt import run_script
    run_script(QurtRamfsBuilder().main)

# Signatures of the files that this depends on
# 46a31eb420b1999d63d43a1df42e94ca /local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/core/kernel/qurt/build/adsp_link/qdsp6/670.adsp.prod/install/ADSPv65MP/scripts/lib/qurt.py
