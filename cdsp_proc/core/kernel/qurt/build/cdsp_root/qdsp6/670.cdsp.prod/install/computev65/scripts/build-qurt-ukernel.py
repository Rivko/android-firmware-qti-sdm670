#! /usr/bin/python
#=============================================================================
#
#                                    build-qurt-ukernel.py
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
#
#  Build the QuRT microkernel.
#
#

def run_program(cmd):
    import subprocess
    import sys
    print ' '.join(cmd)
    prog = subprocess.Popen(cmd)
    if prog.wait() != 0:
        sys.exit(1)

class SecureSection(object):
    #
    #  Define an ELF section to be created by the pre-link
    #   step.  Provides methods to retrieve both the linker
    #   script definition of the section and the assembly
    #   language code to put delimiter symbols at the
    #   start and end of the section, and to emit the
    #   start and end locations into .qskernel_ranges.
    #
    #  Example usage:
    #   foo = SecureSection('.foogoo', '*(.foo .foo.*)', '*(.goo .goo.*)')
    #
    #   This collects all input sections of name .foo and .foo.*
    #    first, and then all input sections of name .goo and .goo.*
    #    second, and concatenates them all into an output section
    #    named .foogoo.  Two words will be emitted into the
    #    .qskernel_ranges section -- the first word will be the
    #    starting virtual address of the section .foogoo, and the
    #    second word will be the ending virtual address of the
    #    section .foogoo.
    #
    def __init__(self, section_name, *contents, **kwargs):
        self.section_name = section_name
        self.contents = contents
        self.NOBITS = False
        self.__dict__.update(kwargs)
    def get_section_spec(self):
        ret = []
        ret.append('   %s_vstart : {' % self.section_name)
        ret.append('      *(%s_vstart)' % self.section_name)
        ret.append('   }')
        ret.append('   %s : {' % self.section_name)
        for C in self.contents:
            ret.append('      %s' % str(C))
        ret.append('   }')
        ret.append('   %s_vend : {' % self.section_name)
        ret.append('      *(%s_vend)' % self.section_name)
        ret.append('   }')
        ret.append('')
        return '\n'.join(ret)
    def get_range_code(self):
        #
        #  The "range code" for a section puts a structure of the
        #   following format into .qskernel_ranges:
        #
        #   struct qskernel_range {
        #     uint32_t vstart;              /* Virtual address of the beginning of the section */
        #     uint32_t vend;                /* Virtual address of the end of the section */
        #     uint32_t flags;               /* A set of flags for this section */
        #     uint32_t reserved;            /* Reserved for future use */
        #     uint64_t load_physaddr;       /* Physical address where the section contents are loaded */
        #     uint64_t run_physaddr;        /* Physical address where the section is located at runtime */
        #   };
        #
        #   The physaddr fields are necessarily filled in later after the final link has been
        #    completed.  At this step of the build process, we fill them with zero.  Note that
        #    the physaddr fields are 64-bit values; the Hexagon supports 36-bit physical addresses,
        #    and this structure will permit a kernel to be (mostly) booted into areas outside the
        #    bottom 4GB of physical address space.  Some of the kernel must remain in the lower
        #    4GB to permit execution while the MMU is disabled, but other kernel sections could
        #    be loaded outside the low 4GB, or relocated to outside the low 4GB during boot.
        #
        #   Note that for ZI sections, the load_physaddr is set to be the same as the run_physaddr
        #    field, but the ZI flag is set to indicate that the section must be zeroed during boot.
        #
        #   Meaning of the flags fields -- some of which may be set here, and some of which may
        #    be set later after the final link has been completed:
        #       Bit 31 -- X (executable bit should be set in permissions?)
        #       Bit 30 -- W (writeable bit should be set in permissions?)
        #       Bit 29 -- R (readable bit should be set in permissions?)
        #       Bit 28 -- U (user-mode bit should be set in permissions?)
        #       Bit  3 -- RECLAIM (physical memory for this section may be reclaimed after cold boot?)
        #       Bit  2 -- RPA_RELOC (run_physaddr is relative to start of the load image?)
        #       Bit  1 -- LPA_RELOC (load_physaddr is relative to start of the load image?)
        #       Bit  0 -- ZI (section should be zero-initialized?)
        #
        #   (If you're wondering why a kernel section might be writeable from outside the kernel --
        #    including even from user mode -- consider the concept of a user-writeable debug buffer.
        #    The buffer itself can be modified from outside the kernel, but the kernel can also
        #    safely read or write the contents of the buffer at any time without worrying about
        #    whether the memory mapping is going to get pulled away, etc.)
        #
        ret = []
        if self.NOBITS:
            bits = 'nobits'
            flags = ((1 << 29) |            # NOBITS sections are readable by the guest-OS only
                     (1 <<  0))             # NOBITS sections should be zero-initialized
        else:
            bits = 'progbits'
            flags = ((1 << 31) |            # PROGBITS sections are executable by non-kernel code
                     (1 << 29) |            # PROGBITS sections are readable by non-kernel code
                     (1 << 28))             # PROGBITS sections are user-mode accessible (for the QURTK_safe_XXX functions)
        ret.append('    .section %s_vstart,"a",@%s' % (self.section_name, bits))
        ret.append('    .equ %s_vstart, .' % self.section_name.lstrip('.'))
        ret.append('    .section %s_vend,"a",@%s' % (self.section_name, bits))
        ret.append('    .equ %s_vend, .' % self.section_name.lstrip('.'))
        ret.append('    .section .qskernel_ranges,"a",@progbits')
        ret.append('    .align 8')
        ret.append('    .int %s_vstart' % self.section_name.lstrip('.'))
        ret.append('    .int %s_vend' % self.section_name.lstrip('.'))
        ret.append('    .int 0x%X' % flags)
        ret.append('    .int 0')
        ret.append('    .quad 0')
        ret.append('    .quad 0')
        ret.append('')
        return '\n'.join(ret)

class BuildKernel:
    def parse_args(self, argv):
        from optparse import OptionParser
        parser = OptionParser()
        parser.add_option('--machine')
        parser.add_option('--compiler')
        parser.add_option('--output')
        opts, args = parser.parse_args(argv[1:])
        self.__dict__.update(opts.__dict__)
        self.args = args

    def main(self, argv):
        import subprocess
        import os

        self.parse_args(argv)

        sections = []
        sections.append(SecureSection('.qskernel_ranges',
                                      '*(.qskernel_ranges)'))
        sections.append(SecureSection('.qskernel_island',
                                      '*cc.a:(.text)',
                                      '*(.text.ukernel.island)',
                                      '*(.rodata.ukernel.island)',
                                      '*(.data.ukernel.island)'))
        sections.append(SecureSection('.qskernel_zi_island',
                                      '*(.bss.ukernel.island)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_eip',
                                      '*(.rodata.export.eip)',
                                      '*(.qskernel_eip)'))
        sections.append(SecureSection('.qskernel_exports',
                                      '*(.qskernel_exports)'))
        sections.append(SecureSection('.qskernel_eip_build',
                                      '*(.qskernel_eip_build)'))
        sections.append(SecureSection('.qskernel_coldboot',
                                      '*(.qskernel_coldboot)'))
        sections.append(SecureSection('.qskernel_zi_coldboot',
                                      '*(.qskernel_zi_coldboot)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_tcm',
                                      '*(.qskernel_tcm)'))
        sections.append(SecureSection('.qskernel_zi_tcm',
                                      '*(.qskernel_zi_tcm)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_zipages_main',
                                      '*(.qskernel_zipages_main)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_zipages_island',
                                      '*(.qskernel_zipages_island)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_zipages_tcm',
                                      '*(.qskernel_zipages_tcm)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_zipages_coldboot',
                                      '*(.qskernel_zipages_coldboot)',
                                      NOBITS=True))
        sections.append(SecureSection('.qskernel_main',
                                      '*(.start)',
                                      '*(.text.ukernel.main)',
                                      '*(.text)',
                                      '*(.rodata.ukernel.main)',
                                      '*(.rodata)',
                                      '*(.data.ukernel.main)',
                                      '*(.data)',
                                      '*(.sdata)'))
        sections.append(SecureSection('.qskernel_zi_main',
                                      '*(.bss.ukernel.main)',
                                      '*(.bss)',
                                      '*(.sbss)',
                                      NOBITS=True))

        lcsname = os.path.join(os.path.dirname(self.output), 'ukernel.lcs')
        fo = open(lcsname, 'w')
        fo.write('SECTIONS {\n')
        for S in sections:
            fo.write(S.get_section_spec())
        fo.write('}\n')
        fo.close()

        rangesname = os.path.join(os.path.dirname(self.output), 'ukernel_ranges.s')
        fo = open(rangesname, 'w')
        for S in sections:
            fo.write(S.get_range_code())
        fo.close()

        cmd = [self.compiler, self.machine, '-o', self.output, '-T', lcsname, '-Wl,-r', '-nostdlib', rangesname] + self.args + ['-lgcc']
        run_program(cmd)
        return 0

if __name__ == '__main__':
    from lib.qurt import run_script
    run_script(BuildKernel().main)


# Signatures of the files that this depends on
# 46a31eb420b1999d63d43a1df42e94ca /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/qurt.py
