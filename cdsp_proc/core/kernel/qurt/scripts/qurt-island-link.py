#! /usr/bin/python
#=============================================================================
#
#                                    qurt-island-link.py
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

'''
    To use this file to create a linker command script from within SCons, do something like this:

    import imp
    qurt_island_py_file = '%s/qurt-island-link.py' % QURT_SCRIPT_DIRECTORY
    qurtisland = imp.load_source('qurtisland', qurt_island_py_file)
    qurtisland.create_lcs(output_file_name, template_file_name, None, island_spec_file_name)
'''

import re

def process_conditionals(script_string, kw):
    #
    #  Allow conditionals in a linker script template.
    #
    #  The format of a conditional is like this:
    #    /* IF ( cond ) */
    #      xxx
    #    /* ENDIF ( cond ) */
    #
    #  The IF and ENDIF must match up, and the ENDIF must carry the same cond string.
    #  White space is ignored within these conditionals, including newlines.
    #
    #  Valid cond values are either of the form key or !key -- a conditional of the
    #   form key is true if a non-zero value for the key was passed in the kw dictionary.
    #   A conditional of the form !key is the opposite of key, including the case
    #   where the key was not passed at all.  The key will be looked up as a string,
    #   so something like IF (1) will only be true if kw includes an item with the
    #   key '1' and a non-zero value...
    #
    #  Note that !!x is the same as x.  There's really no need to use this construct,
    #   but it's worth noting that there's no way to test a key that begins with the
    #   '!' character...
    #

    def test_cond(cond):
        if cond.startswith('!'):
            return not test_cond(cond[1:])
        return bool(kw.get(cond,False))

    #
    #  Parse script_string into a list of non-comment strings and comment strings
    #

    comment = re.compile(r'(/\*.*?\*/)', re.DOTALL)     # Regular expression to match a comment
    match_if = re.compile(r'/\*IF\((.*)\)\*/')          # Regular expression to match IF(cond), returns cond
    match_endif = re.compile(r'/\*ENDIF\((.*)\)\*/')    # Regular expression to match ENDIF(cond), returns cond
    parsed = comment.split(script_string)

    ret = []

    #
    #  parsed[1], parsed[3], parsed[5], etc., are the comments
    #
    checkit = False                                     # Don't check parsed[0]; after that, flip the sense for each one
    active_stack = [True]                               # List containing True/False for each active conditional
    cond_stack = [None]                                 # List containing cond values for each active conditional
    for C in parsed:
        skipit = (False in active_stack)                # If there's an active False conditional, skip this one

        if checkit:
            raw = ''.join(C.split())                    # Strip all whitespace

            tmp = match_if.match(raw)                   # Check for IF
            if tmp:
                cond = tmp.group(1)                     # Get cond string
                cond_stack.append(cond)                 # Add it to list of active conditionals
                active_stack.append(test_cond(cond))    # Add the value to the active_stack
                skipit = True                           # Always skip passing through IF
                print active_stack
                print cond_stack

            tmp = match_endif.match(raw)                # Check for ENDIF
            if tmp:
                cond = tmp.group(1)                     # Get cond string
                if cond != cond_stack.pop():
                    raise Exception('Mismatched conditionals in linker script template: %s' % C)
                active_stack.pop()
                skipit = True                           # Always skip passing through ENDIF

        if not skipit:
            ret.append(C)

        checkit = not checkit

    if len(cond_stack) > 1:
        raise Exception('Unterminated conditionals in linker script template: %s' % cond_stack[1:])

    return ''.join(ret)

def create_lcs(linker_script_name, template_script_name, full_file_names, *spec_file_list, **kw):
    '''
    Read in all of the named spec files and the template script.
    Write out the final linker script.
    If full_file_names is a non-empty list, then basenames which are retrieved from the spec_file_list
    are expanded to match any full names in the list.  If full_file_names is an empty list or None,
    then the names in the spec_file_list are not modified at all.  Example:
    If the spec file lists libfoobar.a, and if the full_file_names array contains an entry with the
    string '/the/path/to/libfoobar.a', then the full name is substituted for libfoobar.a when
    filling in the template.  This is intended so that test cases can run with a constant island
    spec file which contains only base names even if the linker command line contains full path names.
    '''

    if full_file_names:
        import os.path
        def cpath(s):
            for a in full_file_names:
                if os.path.basename(a) == s:
                    return a
            return '*%s' % s
    else:
        def cpath(s):
            return s

    island_sections = []
    island_specs = []

    for fname in spec_file_list:
        f = open(fname, 'r')
        for specline in f:
            temp = specline.split()
            if len(temp):
                if temp[0].startswith('#'):
                    # Treat it as a comment
                    pass
                elif len(temp) == 3:
                    if temp[2] != '*':
                        island_sections.append(temp)
                    else:
                        island_specs.append(temp)
                else:
                    raise Exception('Island spec file has bad format; each non-blank line must have 3 whitespace separated fields')
        f.close()

    # Get the sorted list of section names we've been handed.

    section_names = sorted(set([s[2] for s in island_sections]))

    # Read in the linker script file

    f = open(template_script_name, 'r')
    script_file = f.read()
    f.close()

    script_file = process_conditionals(script_file, kw)

    import re
    if re.search('(/\*\\s*(?:SECURE_KERNEL)\\s*\*/)', script_file):
        return create_secure_lcs(linker_script_name, template_script_name, full_file_names, *spec_file_list, **kw)

    #
    # Define the anchor comments we're able to scan for and replace
    #
    anchors = '''
        ISLAND_EXCLUDE
        ISLAND_TEXT
        ISLAND_RODATA
        ISLAND_DATA
        ISLAND_RW_SECTIONS
        ISLAND_RO_SECTIONS
        QURT_ISLAND_UKERNEL
        QURT_MAIN_UKERNEL
        QURT_START
        START_SECTION
    '''
    regex_string = '(/\*\\s*(?:%s)\\s*\*/)' % '|'.join(anchors.split())

    tokens = re.split(regex_string, script_file)

    script_file = ''

    while len(tokens) >= 2:
        script_file += tokens[0]
        if 'QURT_ISLAND_UKERNEL' in tokens[1]:
            if island_sections or island_specs:
                # There's stuff going into the island
                #script_file += ' .rela.ukernel : { *(.rela.*.ukernel.*) }\n'
                script_file += ' .ukernel.island : { *(*.ukernel.island *.export.island) }'
            else:
                # There's nothing going into the island; don't put ukernel there
                pass
        elif 'QURT_MAIN_UKERNEL' in tokens[1]:
            if island_sections or island_specs:
                # The island ukernel stuff goes in the island, not here
                script_file += ' .ukernel.main : { *(*.ukernel.main *.export.main) }'
            else:
                # The island ukernel stuff goes here, not in an unused island
                #script_file += ' .rela.ukernel : { *(.rela.*.ukernel.*) }\n'
                script_file += ' .ukernel.main : { *(*.ukernel.main *.export.main *.ukernel.island *.export.island) }'
        elif 'QURT_START' in tokens[1]:
            script_file += ' .qurtstart : { __qurt_start = .;}\n . = .;\n'
        elif 'START_SECTION' in tokens[1]:
            script_file += ' .start : { KEEP(*(.start)) }\n'
        elif 'ISLAND_EXCLUDE' in tokens[1]:
            script_file += ' EXCLUDE_FILE(\n'
            for i in island_specs:
                if i[0] == '*' and i[1] != '*':
                    # Single object file
                    script_file += '      %s\n' % cpath(i[1])
                elif i[1] == '*' and i[0] != '*':
                    # Single library
                    script_file += '      %s:\n' % cpath(i[0])
                elif i[1] != '*' and i[0] != '*':
                    # Object within a library
                    script_file += '      %s:%s\n' % (cpath(i[0]), i[1])
            script_file += '   ) '
        elif '_SECTIONS' in tokens[1]:
            constraint = 'ONLY_IF_%s' % tokens[1].split('_')[1]
            for nm in section_names:
                if (nm == '.text' or nm.startswith('.text.')):
                    # Treat these specially -- avoid the constraint
                    if constraint == 'ONLY_IF_RW':
                        continue
                    section_constraint = ''
                elif (nm == '.data' or nm.startswith('.data.')):
                    # Treat these specially -- avoid the constraint
                    if constraint == 'ONLY_IF_RO':
                        continue
                    section_constraint = ''
                else:
                    section_constraint = constraint
                script_file += '   %s.IS.island : %s {\n' % (nm, section_constraint)
                for i in island_sections:
                    if i[2] == nm:
                        if i[0] == '*' and i[1] != '*':
                            # Single object file
                            script_file += '      %s(%s)\n' % (cpath(i[1]), nm)
                        elif i[1] == '*' and i[0] != '*':
                            # Single library
                            script_file += '      %s:(%s)\n' % (cpath(i[0]), nm)
                        elif i[1] != '*' and i[0] != '*':
                            # Object within a library
                            script_file += '      %s:%s(%s)\n' % (cpath(i[0]), i[1], nm)
                        else:
                            # All objects, all libraries
                            script_file += '      *(%s)\n' % (nm)
                script_file += '   }\n'
        else:
            script_file += ' /*START_ISLAND*/\n'
            if 'ISLAND_TEXT' in tokens[1]:
                sections = ['.text .text.*']
                script_file += ' *(.text.qurtos.island)\n'
            elif 'ISLAND_RODATA' in tokens[1]:
                sections = ['.rodata .rodata.*']
                script_file += ' *(.rodata.qurtos.island)\n'
            elif 'ISLAND_DATA' in tokens[1]:
                sections = ['.bss .bss.* COMMON', '.data .data.*', '.sbss .sbss.* .scommon .scommon.*', '.sdata .sdata.*']
                script_file += ' *(.bss.qurtos.island)\n'
                script_file += ' *(.data.qurtos.island)\n'
            for sec in sections:
                for i in island_specs:
                    if i[0] == '*' and i[1] != '*':
                        # Single object file
                        script_file += '      %s(%s)\n' % (cpath(i[1]),sec)
                    elif i[1] == '*' and i[0] != '*':
                        # Single library
                        script_file += '      %s:(%s)\n' % (cpath(i[0]),sec)
                    elif i[1] != '*' and i[0] != '*':
                        # Object within a library
                        script_file += '      %s:%s(%s)\n' % (cpath(i[0]), i[1], sec)
            script_file += ' /*END_ISLAND*/ '
        tokens = tokens[2:]

    script_file += tokens[0]

    f = open(linker_script_name, 'w')
    f.write(script_file+'\0')   # Zero terminate the linker script file so it can be reliably mmap'd and treated as a C string.
    f.close()

def create_secure_lcs(linker_script_name, template_script_name, full_file_names, *spec_file_list, **kw):
    '''
    Read in all of the named spec files and the template script.
    Write out the final linker script.
    If full_file_names is a non-empty list, then basenames which are retrieved from the spec_file_list
    are expanded to match any full names in the list.  If full_file_names is an empty list or None,
    then the names in the spec_file_list are not modified at all.  Example:
    If the spec file lists libfoobar.a, and if the full_file_names array contains an entry with the
    string '/the/path/to/libfoobar.a', then the full name is substituted for libfoobar.a when
    filling in the template.  This is intended so that test cases can run with a constant island
    spec file which contains only base names even if the linker command line contains full path names.
    '''

    if full_file_names:
        import os.path
        def cpath(s):
            for a in full_file_names:
                if os.path.basename(a) == s:
                    return a
            return '*%s' % s
    else:
        def cpath(s):
            return s

    island_sections = []
    island_specs = []

    for fname in spec_file_list:
        f = open(fname, 'r')
        for specline in f:
            temp = specline.split()
            if len(temp):
                if temp[0].startswith('#'):
                    # Treat it as a comment
                    pass
                elif len(temp) == 3:
                    if temp[2] != '*':
                        island_sections.append(temp)
                    else:
                        island_specs.append(temp)
                else:
                    raise Exception('Island spec file has bad format; each non-blank line must have 3 whitespace separated fields')
        f.close()

    # Get the sorted list of section names we've been handed.

    section_names = sorted(set([s[2] for s in island_sections]))

    # Read in the linker script file

    f = open(template_script_name, 'r')
    script_file = f.read()
    f.close()

    script_file = process_conditionals(script_file, kw)

    #
    # Define the anchor comments we're able to scan for and replace
    #
    anchors = '''
        ISLAND_EXCLUDE
        ISLAND_TEXT
        ISLAND_RODATA
        ISLAND_DATA
        ISLAND_RW_SECTIONS
        ISLAND_RO_SECTIONS
        QURT_ISLAND_UKERNEL
        QURT_TCM_UKERNEL
        QURT_MAIN_UKERNEL
        QURT_COLDBOOT_UKERNEL
        QURT_START
        SECURE_KERNEL
    '''
    regex_string = '(/\*\\s*(?:%s)\\s*\*/)' % '|'.join(anchors.split())

    import re
    tokens = re.split(regex_string, script_file)

    # See if it's a secure build, and if it's an island build
    secure_kernel = False
    island_build = False
    for m in tokens[1::2]:
        if 'SECURE_KERNEL' in m:
            secure_kernel = True
        if 'ISLAND' in m:
            island_build = True

    script_file = ''

    def pretty(strlist):
        return '\n'.join(strlist+[''])
    def feature(tokens, tag):
        for m in tokens[1::2]:
            if tag in m:
                return True
        return False
    def decl(s):
        return [' %s : { KEEP(*(%s)) }' % (s,s)]
    align_4KB                  = ' . = ALIGN(4K);'
    align_1MB                  = ' . = ALIGN(1M);'
    align_coldboot             = ' . = ADDR(.qskernel_main) + (ADDR(.qskshadow_zipages_coldboot) - ADDR(.start));'
    start                      = [' .qstart : { KEEP(*crt0.o(.start)) }']
    qskernel_main              = decl('.qskernel_main')
    qskernel_island            = decl('.qskernel_island')
    qskernel_tcm               = decl('.qskernel_tcm')
    qskernel_coldboot          = decl('.qskernel_coldboot')
    qskernel_zi_main           = decl('.qskernel_zi_main')
    qskernel_zi_island         = decl('.qskernel_zi_island')
    qskernel_zi_tcm            = decl('.qskernel_zi_tcm')
    qskernel_zi_coldboot       = decl('.qskernel_zi_coldboot')
    qskernel_zipages_main      = decl('.qskernel_zipages_main')
    qskernel_zipages_island    = decl('.qskernel_zipages_island')
    qskernel_zipages_tcm       = decl('.qskernel_zipages_tcm')
    qskernel_zipages_coldboot  = decl('.qskernel_zipages_coldboot')
    qskernel_eip               = decl('.qskernel_eip')
    qskernel_exports           = decl('.qskernel_exports')
    qskernel_ranges            = decl('.qskernel_ranges')
    qskshadow_main             = decl('.qskshadow_main')
    qskshadow_island           = decl('.qskshadow_island')
    qskshadow_tcm              = decl('.qskshadow_tcm')
    qskshadow_coldboot         = decl('.qskshadow_coldboot')
    qskshadow_zi_main          = decl('.qskshadow_zi_main')
    qskshadow_zi_island        = decl('.qskshadow_zi_island')
    qskshadow_zi_tcm           = decl('.qskshadow_zi_tcm')
    qskshadow_zi_coldboot      = decl('.qskshadow_zi_coldboot')
    qskshadow_zipages_main     = decl('.qskshadow_zipages_main')
    qskshadow_zipages_island   = decl('.qskshadow_zipages_island')
    qskshadow_zipages_tcm      = decl('.qskshadow_zipages_tcm')
    qskshadow_zipages_coldboot = decl('.qskshadow_zipages_coldboot')
    qskshadow_eip              = decl('.qskshadow_eip')
    qskshadow_exports          = decl('.qskshadow_exports')
    qskshadow_ranges           = decl('.qskshadow_ranges')
    qskstart                   = [' .qskstart : { KEEP(*(.start)) }']
    #
    #  Build up each of our anchors
    #
    island_shadow = []
    island_decl = []
    tcm_shadow = []
    tcm_decl = []
    main_shadow = []
    main_decl = []
    coldboot_shadow = []
    coldboot_decl = []
    kspace_decl = []
    if feature(tokens,'QURT_ISLAND_UKERNEL'):
        island_shadow.append(align_1MB)
        island_shadow.append(' .ukernel.island : {}')
        island_shadow.append(' . = ADDR(.ukernel.island);')
        island_shadow.append(qskshadow_zipages_island.pop())
        island_shadow.append(qskshadow_zi_island.pop())
        island_shadow.append(qskshadow_island.pop())
        island_decl.append(align_1MB)
        island_decl.append(qskernel_zipages_island.pop())
        island_decl.append(qskernel_zi_island.pop())
        island_decl.append(qskernel_island.pop())
    if feature(tokens,'QURT_TCM_UKERNEL'):
        tcm_shadow.append(align_1MB)
        tcm_shadow.append(qskshadow_zipages_tcm.pop())
        tcm_shadow.append(qskshadow_zi_tcm.pop())
        tcm_shadow.append(qskshadow_tcm.pop())
        tcm_decl.append(align_1MB)
        tcm_decl.append(qskernel_zipages_tcm.pop())
        tcm_decl.append(qskernel_zi_tcm.pop())
        tcm_decl.append(qskernel_tcm.pop())
    if feature(tokens,'QURT_COLDBOOT_UKERNEL'):
        coldboot_shadow.append(align_4KB)
        coldboot_shadow.append(qskshadow_zipages_coldboot.pop())
        coldboot_shadow.append(qskshadow_zi_coldboot.pop())
        coldboot_shadow.append(qskshadow_coldboot.pop())
        coldboot_decl.append(align_coldboot)
        coldboot_decl.append(qskernel_zipages_coldboot.pop())
        coldboot_decl.append(qskernel_zi_coldboot.pop())
        coldboot_decl.append(qskernel_coldboot.pop())
    main_shadow.append(align_1MB)
    main_shadow.extend(start)
    main_shadow.extend(qskshadow_main)
    main_shadow.extend(qskshadow_ranges)
    main_shadow.extend(qskshadow_island)
    main_shadow.extend(qskshadow_tcm)
    main_shadow.extend(qskshadow_coldboot)
    main_shadow.extend(qskshadow_zi_main)
    main_shadow.extend(qskshadow_zi_island)
    main_shadow.extend(qskshadow_zi_tcm)
    main_shadow.extend(qskshadow_zi_coldboot)
    main_shadow.extend(qskshadow_zipages_main)
    main_shadow.extend(qskshadow_zipages_island)
    main_shadow.extend(qskshadow_zipages_tcm)
    main_shadow.extend(qskshadow_zipages_coldboot)
    main_shadow.append(align_4KB)
    main_shadow.extend(qskshadow_eip)
    main_shadow.extend(qskshadow_exports)
    main_shadow.append(align_4KB)
    main_shadow.extend(qskstart)
    main_shadow.append(align_4KB)
    main_decl.append(' .qsvspace : {')
    main_decl.append('   . = ALIGN(8M);')
    main_decl.append('   . = . + 0x800000;')
    main_decl.append(' }')
    main_decl.append('   . = 0xFE000000;')
    main_decl.extend(qskernel_main)
    main_decl.extend(qskernel_ranges)
    main_decl.extend(qskernel_island)
    main_decl.extend(qskernel_tcm)
    main_decl.extend(qskernel_coldboot)
    main_decl.append(' __kernel_bss_start = .;')
    main_decl.extend(qskernel_zi_main)
    main_decl.extend(qskernel_zi_island)
    main_decl.extend(qskernel_zi_tcm)
    main_decl.extend(qskernel_zi_coldboot)
    main_decl.extend(qskernel_zipages_main)
    main_decl.extend(qskernel_zipages_island)
    main_decl.extend(qskernel_zipages_tcm)
    main_decl.extend(qskernel_zipages_coldboot)
    main_decl.append(' __kernel_bss_end = .;')
    main_decl.append(align_4KB)
    main_decl.extend(qskernel_eip)
    main_decl.extend(qskernel_exports)
    main_decl.append(align_4KB)
    main_decl.append(' .qskernel_eip_build : { KEEP(*(.qskernel_eip_build)) }')
    kspace_decl.append(' .qskernel_vspace : {')
    kspace_decl.append('   . = ADDR(.qskernel_main)+0x800000;')
    kspace_decl.append(' }')

    while len(tokens) >= 2:
        script_file += tokens[0]
        if 'SECURE_KERNEL' in tokens[1]:
            script_file += pretty(main_decl+coldboot_decl+island_decl+tcm_decl+kspace_decl)
        elif 'QURT_ISLAND_UKERNEL' in tokens[1]:
            script_file += pretty(island_shadow)
        elif 'QURT_TCM_UKERNEL' in tokens[1]:
            script_file += pretty(tcm_shadow)
        elif 'QURT_MAIN_UKERNEL' in tokens[1]:
            script_file += pretty(main_shadow)
        elif 'QURT_COLDBOOT_UKERNEL' in tokens[1]:
            script_file += pretty(coldboot_shadow)
        elif 'QURT_START' in tokens[1]:
            script_file += ' .qurtstart : {}\n . = ADDR(.qurtstart);\n'
        elif 'ISLAND_EXCLUDE' in tokens[1]:
            script_file += ' EXCLUDE_FILE(\n'
            for i in island_specs:
                if i[0] == '*' and i[1] != '*':
                    # Single object file
                    script_file += '      %s\n' % cpath(i[1])
                elif i[1] == '*' and i[0] != '*':
                    # Single library
                    script_file += '      %s:\n' % cpath(i[0])
                elif i[1] != '*' and i[0] != '*':
                    # Object within a library
                    script_file += '      %s:%s\n' % (cpath(i[0]), i[1])
            script_file += '   ) '
        elif '_SECTIONS' in tokens[1]:
            constraint = 'ONLY_IF_%s' % tokens[1].split('_')[1]
            for nm in section_names:
                if (nm == '.text' or nm.startswith('.text.')):
                    # Treat these specially -- avoid the constraint
                    if constraint == 'ONLY_IF_RW':
                        continue
                    section_constraint = ''
                elif (nm == '.data' or nm.startswith('.data.')):
                    # Treat these specially -- avoid the constraint
                    if constraint == 'ONLY_IF_RO':
                        continue
                    section_constraint = ''
                else:
                    section_constraint = constraint
                script_file += '   %s.IS.island : %s {\n' % (nm, section_constraint)
                for i in island_sections:
                    if i[2] == nm:
                        if i[0] == '*' and i[1] != '*':
                            # Single object file
                            script_file += '      %s(%s)\n' % (cpath(i[1]), nm)
                        elif i[1] == '*' and i[0] != '*':
                            # Single library
                            script_file += '      %s:(%s)\n' % (cpath(i[0]), nm)
                        elif i[1] != '*' and i[0] != '*':
                            # Object within a library
                            script_file += '      %s:%s(%s)\n' % (cpath(i[0]), i[1], nm)
                        else:
                            # All objects, all libraries
                            script_file += '      *(%s)\n' % (nm)
                script_file += '   }\n'
        else:
            if 'ISLAND_TEXT' in tokens[1]:
                sections = ['.text .text.*']
            elif 'ISLAND_RODATA' in tokens[1]:
                sections = ['.rodata .rodata.*']
            elif 'ISLAND_DATA' in tokens[1]:
                sections = ['.bss .bss.* COMMON', '.data .data.*', '.sbss .sbss.* .scommon .scommon.*', '.sdata .sdata.*']
            script_file += ' /*START_ISLAND*/\n'
            for sec in sections:
                for i in island_specs:
                    if i[0] == '*' and i[1] != '*':
                        # Single object file
                        script_file += '      %s(%s)\n' % (cpath(i[1]),sec)
                    elif i[1] == '*' and i[0] != '*':
                        # Single library
                        script_file += '      %s:(%s)\n' % (cpath(i[0]),sec)
                    elif i[1] != '*' and i[0] != '*':
                        # Object within a library
                        script_file += '      %s:%s(%s)\n' % (cpath(i[0]), i[1], sec)
            script_file += ' /*END_ISLAND*/ '
        tokens = tokens[2:]

    script_file += tokens[0]

    f = open(linker_script_name, 'w')
    f.write(script_file+'\0')   # Zero terminate the linker script file so it can be reliably mmap'd and treated as a C string.
    f.close()

# Each row of the parser spec is intended to extract some information from the linker command line
#  that we're passed.
# The elements of each row are (in order):
#  Short form of the linker option (or None if no short form)
#  Long form of the linker option (or None if no long form)
#  Key in our parser dictionary under which to accumulate matching options
#  Boolean for whether to strip the option or to pass it through
#   (If we're extending or modifying options, this should be True; if
#    we're only extracting info, this should be False)

parser_spec = [
    ['-o', '--output',      'output_file', False],    # Where the linker output goes.  We store temp files in that directory.
    ['-T', '--script',      'script_file', True ],    # The linker script.  We read this and create a modified version.
    [None, '--island-spec', 'ispec_file',  True ],    # The island spec.  Determines how we modify the linker script.
    ]

def qurt_island_fixup(original_args, passthru_args, parsed):
    for p in parser_spec:
        if not p[2] in parsed or len(parsed[p[2]]) != 1:
            raise Exception('qurt-island-link.py cannot parse arguments correctly')
        parsed[p[2]] = parsed[p[2]][0]

    import os.path
    outfile_name = os.path.join(os.path.dirname(parsed['output_file']),'qurt_island_link.lcs')

    create_lcs(outfile_name, parsed['script_file'], passthru_args, parsed['ispec_file'])

    passthru_args.insert(1, '--script=%s' % outfile_name)

    return passthru_args

def qurt_island_link(argv):
    def parse_match(key, strip, arg):
        if key:
            return (key, strip, arg)                                # A previous argument was still pending
        for p in parser_spec:
            if arg == p[0] or arg == p[1]:
                return (p[2], p[3], None)                           # Parsed it, but no string available yet
            if p[0] and arg.startswith(p[0]):
                return (p[2], p[3], arg.replace(p[0],'',1))         # Parsed it, return string
            if p[1] and arg.startswith(p[1]+'='):
                return (p[2], p[3], arg.replace(p[1]+'=','',1))     # Parsed it, return string
        return (None, None, None)

    try:
        original_args = argv[1:]
        passthru_args = []
        parsed = dict()
        strip = None
        key = None
        for arg in original_args:
            (key, strip, value) = parse_match(key, strip, arg)
            if value:
                parsed.setdefault(key,[]).append(value)
                key = None
            if not strip:
                passthru_args.append(arg)

        use_args = qurt_island_fixup(original_args, passthru_args, parsed)

        print ' '.join(use_args)

        import subprocess
        return subprocess.Popen(use_args).wait()
    except (SystemExit, KeyboardInterrupt):
        raise
    except Exception, err:
        import traceback
        traceback.print_exc()
    except:
        raise
    return 1

if __name__ == '__main__':
    import sys
    sys.exit(qurt_island_link(sys.argv))
