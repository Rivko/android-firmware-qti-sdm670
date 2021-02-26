#=============================================================================
#
#                                    build_qurt_config.py
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
# Build cust_config.o
#

from qurt import *

def get_sections(secure=False):
    from cfgvars import Script
    S = Script()

    S.add_section('.rodata.eip.build',
                  NAME='eip_*',
                  EXACT=True)
    S.add_section('.qurtos.island',
                  ISLAND=True, QURTOS=True)
    S.add_section('.qurtos.main',
                  ISLAND=False, QURTOS=True)
    S.add_section('.ukernel.island',
                  ISLAND=True)
    S.add_section('')

    return S.tostring()

def find_compiler(cfg):
    # Return the string which executes the compiler
    # Try the requested LLVM setting first, then try the opposite
    return qurt_get_compiler(cfg.tools_path)

def build_qurt_config(cfg):
    import os, re, subprocess, sys

    if cfg.secured_kernel or True:
        return build_secure_qurt_config(cfg)

    # Do basic handling of the arguments
    compiler = find_compiler(cfg)
    if cfg.build_flags:
        build_flags = cfg.build_flags
    else:
        build_flags = []
    if cfg.asic:
        asic = ['-m%s' % cfg.asic]
    else:
        asic = []
    if cfg.object_path:
        object_path = cfg.object_path
    else:
        object_path = '.'
    cfile_path = cfg.cfile_path

    cust_config_o = cfg.object_path
    dir_path, cust_config_o_file_name = os.path.split(object_path)
    cust_config_main_f_name = cust_config_o_file_name.split(".")[0] + "_main.o"
    cust_config_island_f_name = cust_config_o_file_name.split(".")[0] + "_island.o"
    object_path_main = os.path.join(dir_path, cust_config_main_f_name)
    object_path_island = os.path.join(dir_path, cust_config_island_f_name)

    compiler_command = [compiler, '-g', '-nostdinc', '-O2', '-G0'] + asic + build_flags

    if '_main.' in os.path.basename(object_path):
        # Assume caller asked for the "main" (non-island) object only
        compiler_command += ['-DMAIN_ONLY',
                             '-o', object_path,
                             '-c', cfile_path]
    elif '_island.' in os.path.basename(object_path):
        # Assume caller asked for the "island" object only
        compiler_command += ['-DISLAND_ONLY',
                             '-o', object_path,
                             '-c', cfile_path]
    else:
        if '.' in os.path.basename(object_path):
            # Add the _main or the _island before the final . in the name
            object_path_main = '_main.'.join(object_path.rsplit('.',1))
            object_path_island = '_island.'.join(object_path.rsplit('.',1))
        else:
            # Add the _main or the _island after the name
            object_path_main = object_path+'_main'
            object_path_island = object_path+'_island'
        compiler_command = (compiler_command + ['-o', object_path, '-c', cfile_path, '&&'] +
                            compiler_command + ['-DMAIN_ONLY', '-o', object_path_main, '-c', cfile_path, '&&'] +
                            compiler_command + ['-DISLAND_ONLY', '-o', object_path_island, '-c', cfile_path])

    while '&&' in compiler_command:
        i = compiler_command.index('&&')
        sub_command = compiler_command[:i]
        compiler_command = compiler_command[i+1:]
        print ' '.join(sub_command)
        if subprocess.Popen(sub_command).wait():
            raise Exception('Unable to compile configuration object')

    print ' '.join(compiler_command)

    if subprocess.Popen(compiler_command).wait():
        raise Exception('Unable to compile configuration object')

def build_secure_prelink_script(filename, do_island):
    f = open(filename, 'w')
    lcs = '''SECTIONS {
  .text.user.config : {
    *(.text.user.config)
  }
  .rodata.user.config : {
    *(.rodata.user.config)
  }
  .data.user.config : {
    *(.data.user.config)
  }
  .bss.user.config : {
    *(.bss.user.config)
  }
  .qskernel_island : {
    *(.qskernel_island_vstart)
    *(.qskernel_island)
    *(.ukernel.island .ukernel.*.island)
    *(.text.island .text.*.island)
    *(.rodata.island .rodata.*.island)
    *(.data.island .data.*.island)
    *(.sdata.island .sdata.*.island)
    *(.qskernel_island_vend)
  }
  .qskernel_zi_island : {
    *(.qskernel_zi_island_vstart)
    *(.qskernel_zi_island)
    *(.sbss.island .sbss.island.*)
    *(.scommon.island .scommon.island.*)
    *(.bss.island .bss.island.*)
    *(.qskernel_zi_island_vend)
  }
  .qskernel_main : {
    *(.qskernel_main_vstart)
    *(.qskernel_main)
    *(.start)
    *(.text .text.*)
    *(.ukernel.island)
    *(.rodata .rodata.*)
    *(.data .data.*)
    *(.sdata .sdata.*)
    *(.qskernel_main_vend)
  }
  .qskernel_zi_main : {
    *(.qskernel_zi_main_vstart)
    *(.qskernel_zi_main)
    *(.sbss .sbss.*)
    *(.scommon .scommon.*)
    *(.bss .bss.*)
    *(COMMON)
    *(.qskernel_zi_main_vend)
  }
  .qskernel_eip : {
    *(.qskernel_eip_vstart)
    *(.qskernel_eip)
    *(.qskernel_eip_vend)
  }
  .qskernel_exports : {
    *(.qskernel_exports_vstart)
    *(.qskernel_exports)
    *(.qskernel_exports_vend)
  }
  .qskernel_eip_build : {
    *(.qskernel_eip_build)
  }
  .qskernel_coldboot : {
    *(.qskernel_coldboot_vstart)
    *(.qskernel_coldboot)
    *(.qskernel_coldboot_vend)
  }
  .qskernel_zi_coldboot : {
    *(.qskernel_zi_coldboot_vstart)
    *(.qskernel_zi_coldboot)
    *(.qskernel_zi_coldboot_vend)
  }
  .qskernel_tcm : {
    *(.qskernel_tcm_vstart)
    *(.qskernel_tcm)
    *(.qskernel_tcm_vend)
  }
  .qskernel_zi_tcm : {
    *(.qskernel_zi_tcm_vstart)
    *(.qskernel_zi_tcm)
    *(.qskernel_zi_tcm_vend)
  }
  .qskernel_zipages_main : {
    *(.qskernel_zipages_main_vstart)
    *(.qskernel_zipages_main)
    *(.qskernel_zipages_main_vend)
  }
  .qskernel_zipages_island : {
    *(.qskernel_zipages_island_vstart)
    *(.qskernel_zipages_island)
    *(.qskernel_zipages_island_vend)
  }
  .qskernel_zipages_tcm : {
    *(.qskernel_zipages_tcm_vstart)
    *(.qskernel_zipages_tcm)
    *(.qskernel_zipages_tcm_vend)
  }
  .qskernel_zipages_coldboot : {
    *(.qskernel_zipages_coldboot_vstart)
    *(.qskernel_zipages_coldboot)
    *(.qskernel_zipages_coldboot_vend)
  }
  .qskernel_ranges : {
    *(.qskernel_ranges_vstart)
    *(.qskernel_ranges)
    *(.qskernel_ranges_vend)
  }
  .qskernel_vstart_vend : {
    *(.qskernel_*_vstart)
    *(.qskernel_*_vend)
  }
}
'''
    f.write(lcs)
    f.close()

def build_insecure_prelink_script(filename):
    f = open(filename, 'w')
    f.write('SECTIONS {\n%s\n}\n' % (get_sections(),))
    f.close()

def build_kernel_c_source(cfg, filename):
    fo = open(filename, 'w')
    fo.write('#define section(x) used\n')               # Convert section attributes to "used" which is basically a no-op
    fo.write(cfg.vars.get_definitions())
    fo.close()

def build_secure_qurt_config(cfg):
    import os, sys, subprocess
    compiler = find_compiler(cfg)
    kernel_object = None
    if '-DCONFIG_PRIVATE_UKERNEL' in cfg.build_flags:
        kernel_object = os.path.join(os.path.dirname(sys.argv[0]),'Input','qurt_ukernel.o')
    kernel_config = os.path.join(os.path.dirname(cfg.object_path), 'cust_config_1.o')
    kernel_config_c = os.path.join(os.path.dirname(cfg.object_path), 'cust_config_1.c')
    prelinked_kernel = os.path.join(os.path.dirname(cfg.object_path), 'cust_config_3.o')
    if '-DCONFIG_PRIVATE_UKERNEL' in cfg.build_flags:
        prelink_script = os.path.join(os.path.dirname(sys.argv[0]),'Input','private_ukernel.lcs')
    else:
        prelink_script = os.path.join(os.path.dirname(cfg.object_path), 'prelink_script.lcs')
        build_insecure_prelink_script(prelink_script)
    build_kernel_c_source(cfg, kernel_config_c)
    common_flags = ['-g',
                    '-O2',
                    '-G0',
                    '-Wl,-r',
                    '-fno-common',
                    '-ffunction-sections',
                    '-fdata-sections',
                    '-nostdlib',
                    cfg.build_flags,
                    cfg.asic and '-m%s' % cfg.asic]
    run_command(compiler,
                '-DSECURED_KERNEL_YES',
                '-o', kernel_config,
                common_flags,
                '-c', kernel_config_c)
    run_command(compiler,
                '-o', prelinked_kernel,
                common_flags,
                '-T', prelink_script,
                kernel_object,
                kernel_config,
                '-lgcc')
    qurt_secure_elf(cfg.object_path, prelinked_kernel).run()

class qurt_secure_elf(object):
    def __init__(self, outfilename, infilename):
        self.outfilename = outfilename
        self.infilename = infilename
    def section_at(self, index):
        #
        #  Return the section header for (index).
        #  Clips (index) so that we don't fault -- if index is greater than the
        #   number of section headers, return section header for 0.
        #
        if index >= self.ehdr.e_shnum:
            index = 0
        return self.shdr[index]
    def run(self):
        import StringIO
        self.infile = open(self.infilename, 'rb')
        self.ehdr = Elf32Header(self.infile)
        self.infile.seek(self.ehdr.e_shoff)
        self.shdr = [Elf32SectionHeader(self.infile, loc=loc) for loc in range(self.ehdr.e_shnum)]
        for s in self.shdr:
            s.f = self.infile
        strtab_section = self.shdr[self.ehdr.e_shstrndx]
        strtab = strtab_section.contents().read()
        for s in self.shdr:
            s.name = strtab[s.sh_name:].split('\0')[0]
        #
        # For each ALLOC section whose name begins with .qskernel
        #  we create a shadow NOBITS section with the same
        #  attributes, but with the .qskernel replaced with
        #  .qskshadow.  Also, turn off the W bit and turn on
        #  the X bit.
        #
        # While we're in here, set "ring" levels for each section,
        #  with the idea that relocations from an inner ring to an
        #  outer ring are worth reporting as a potential problem.
        #
        # Rings:
        #   0 : .qskernel*island
        #   1 : .qskernel* other than those specifically put to ring 0 or ring 2
        #   2 : .qskernel*eip
        #   2 : .qskernel*ranges
        #   2 : All other sections, plus section 0 (external symbols go in section 0)
        #
        for s in self.shdr[:]:
            s.ring = 2
            if (s.sh_flags & s.SHF_ALLOC) != 0:
                if s.name.startswith('.qskernel'):
                    if s.name == '.qskernel_eip_build':
                        #
                        #  .qskernel_eip_build is treated specially.  Its permissions
                        #   don't matter, and it doesn't need a shadow section.  This
                        #   is because the contents don't survive into the final
                        #   load image.
                        #
                        continue
                    s.ring = 1
                    if s.name.endswith('eip'):
                        s.ring = 2
                    if s.name.endswith('ranges'):
                        s.ring = 2
                    if s.name.endswith('island'):
                        s.ring = 1            # For now, don't try to enforce island in this step
                    s.sh_flags &= ~s.SHF_WRITE
                    s.sh_flags |= s.SHF_EXECINSTR
                    ns = Elf32SectionHeader(s.__dict__)
                    ns.name = s.name.replace('.qskernel','.qskshadow',1)
                    ns.sh_flags &= ~ns.SHF_WRITE
                    ns.sh_flags |= ns.SHF_EXECINSTR
                    ns.sh_type = ns.SHT_NOBITS
                    ns.sh_name = len(strtab)
                    strtab += ns.name + '\0'
                    if s.name == '.qskernel_main':
                        # Subtract 1K from the size.
                        # Report an error if the section requests
                        #  alignment > 1K, or if the size was less
                        #  than 1K to begin with.
                        if ns.sh_size < 1024 or ns.sh_addralign > 1024:
                            raise Exception('%s has improper attributes' % s.name)
                        ns.sh_size -= 1024
                    self.shdr.append(ns)
        strtab_section.sh_size = len(strtab)
        strtab_section.sh_offset = 0
        strtab_section.f = StringIO.StringIO(strtab)
        #
        # Find the symbol table in the object file.
        # Requirements:
        #  * Must be one section header of type SHT_SYMTAB
        #  * Must have the name .symtab
        #  * Must indicate a symbol table entry size of 16 bytes
        #  * Section size must be a multiple of 16 bytes
        #
        symtabs = [s for s in self.shdr if s.sh_type == s.SHT_SYMTAB]
        if len(symtabs) != 1:
            raise Exception('Object must have one SYMTAB section')
        symtab = symtabs[0]
        if symtab.name != '.symtab':
            raise Exception('SYMTAB section must be named .symtab')
        if symtab.sh_entsize != 16:
            raise Exception('SYMTAB entries must be 16 bytes each')
        if symtab.sh_size % 16 != 0:
            raise Exception('SYMTAB size must be a multiple of 16 bytes')
        #
        # Go read all of the symbol table entries.
        #
        symreader = symtab.contents()
        self.syms = [Elf32Sym(symreader) for _ in range(symtab.sh_size/16)]
        strtab = self.shdr[symtab.sh_link].contents().read()
        for s in self.syms:
            if s.st_name:
                s.name = strtab[s.st_name:].split('\0')[0]
            else:
                s.name = ''
        for s in self.syms:
            if s.st_shndx > 0 and s.st_shndx < self.ehdr.e_shnum:
                s.section_name = self.shdr[s.st_shndx].name
            else:
                s.section_name = ''
        #
        # Hide all of the global symbols which are in a .qskernel* section
        #
        for s in self.syms:
            if s.get_binding() == s.STB_GLOBAL and s.section_name.startswith('.qskernel'):
                if s.name == 'start':
                    s.set_binding(s.STB_LOCAL)
                if s.name == 'memcpy':
                    s.set_binding(s.STB_LOCAL)
                if s.name.startswith('__restore_r'):
                    s.set_binding(s.STB_LOCAL)
                if s.name.startswith('__save_r'):
                    s.set_binding(s.STB_LOCAL)
                if s.name.startswith('__deallocframe'):
                    s.set_binding(s.STB_LOCAL)
                if s.name.startswith('__qdsp_'):
                    s.set_binding(s.STB_LOCAL)
                if s.name.startswith('__hexagon_'):
                    s.set_binding(s.STB_LOCAL)
        #
        # Sort all symbols to get the LOCAL symbols at the beginning
        #
        nsyms = [self.syms[0]]
        nsyms.extend(sorted(self.syms[1:], key=lambda s: s.st_info))
        #
        # If a section exists with either the name .qskernel_eip_build
        #  or the name .rodata.eip.build, extend that section by adding
        #  4 zero bytes on the end.  (Adds a terminator to the EIP records.)
        # In addition, define a new symbol QURT_EIP_BUILD which points at
        #  the beginning of the section.
        #
        for s in self.shdr:
            if s.name == '.qskernel_eip_build' or s.name == '.rodata.eip.build':
                payload = s.contents().read() + 4*'\0'
                s.sh_size = len(payload)
                s.sh_offset = 0
                s.f = StringIO.StringIO(payload)
                newsym = Elf32Sym(None,
                                  st_name = len(strtab),
                                  st_value = 0,
                                  st_size = s.sh_size,
                                  st_info = (1 + (Elf32Sym.STB_GLOBAL << 4)),
                                  st_other = 0,
                                  st_shndx = s.loc)
                newsym.name = 'QURT_EIP_BUILD'
                newsym.section_name = s.name
                strtab += newsym.name + '\0'
                self.shdr[symtab.sh_link].sh_size = len(strtab)
                self.shdr[symtab.sh_link].sh_offset = 0
                self.shdr[symtab.sh_link].f = StringIO.StringIO(strtab)
                nsyms.append(newsym)
        #
        # For each symbol, set its "loc" attribute to its new sorted index
        # Also keep track of the highest LOCAL symbol we see.
        #
        count = 0
        localcount = 0
        for s in nsyms:
            s.loc = count
            count += 1
            if s.get_binding() == s.STB_LOCAL:
                localcount = count
        #
        # Update the symtab section with the new count, and a new
        #  pseudo-file so that contents() works correctly with it
        #
        symtab.sh_info = localcount
        symtab_payload = ''.join([s.output() for s in nsyms])
        symtab.sh_size = len(symtab_payload)
        symtab.sh_offset = 0
        symtab.f = StringIO.StringIO(symtab_payload)
        #
        # For each RELA section, read in all RELA elements,
        #  and fix their symbol table indexes.  Store back
        #  the modified info as a pseudo-file.
        #
        for s in self.shdr:
            if s.sh_type == s.SHT_RELA:
                relareader = s.contents()
                relas = [Elf32Rela(relareader) for _ in range(s.sh_size/s.sh_entsize)]
                allsyms = set()
                for r in relas:
                    this_section = self.shdr[s.sh_info]
                    that_sym = self.syms[r.get_sym()]
                    that_section = self.section_at(that_sym.st_shndx)
                    if that_section.ring > this_section.ring:
                        print 'Section %s has a suspicious relocation to section %s : symbol %s' % (this_section.name, that_section.name, that_sym.name)
                        allsyms.add(that_sym.name)
                    r.set_sym(self.syms[r.get_sym()].loc)
                allsyms.discard('')
                if allsyms:
                    print 'Potentially unsafe references to these symbols from %s:' % self.shdr[s.sh_info].name
                    for N in sorted(list(allsyms)):
                        print '  %s' % N
                rela_payload = ''.join([r.output() for r in relas])
                s.sh_offset = 0
                s.f = StringIO.StringIO(rela_payload)
        #
        # Create the output file information
        #
        self.ehdr_out = self.ehdr.copy()
        self.shdr_out = [s.copy() for s in self.shdr]
        payloads = [s.contents() for s in self.shdr_out]
        #
        # Fix up all offsets in the output file
        #
        location = 0
        for s in self.shdr_out:
            oloc = location
            location = alignup(location, max(1,s.sh_addralign))
            s.beforepad = (location-oloc)
            s.sh_offset = location
            if s.sh_type != s.SHT_NOBITS:
                location += s.sh_size
            if location == 0:
                location = self.ehdr_out.e_ehsize
        oloc = location
        location = alignup(location, 4)
        self.ehdr_out.e_shoff = location
        self.ehdr_out.e_shnum = len(self.shdr_out)
        shdr_raw = ((location-oloc)*'\0')+''.join([s.output() for s in self.shdr_out])
        #
        # Ready to write the output file
        #
        fo = open(self.outfilename, 'wb')
        fo.write(self.ehdr_out.output())
        for s in self.shdr_out:
            if s.beforepad:
                fo.write(s.beforepad * '\0')
            fo.write(payloads.pop(0).read())
        fo.write(shdr_raw)
        fo.close()
