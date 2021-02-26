#=============================================================================
#
#                                    genkernel.py
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

import os.path          # Standard Python
import re               # Standard Python
import subprocess       # Standard Python

import qurt_consts      # Auto-generated QuRT script

class QurtVars(object):
    #
    #  Class gets attached to cfg as cfg.vars
    #     from genkernel import QurtVars
    #     QurtVars(cfg)
    #
    def __init__(self, cfg):
        self.cfg = cfg
        cfg.vars = self

    def get_template_file(self):
        #
        #  Look for the C code at ../Input/cust_config.c relative to our script location
        #

        fname = os.path.normpath(os.path.join(os.path.dirname(__file__),
                                              '..',
                                              'Input',
                                              'cust_config.c'))

        fi = open(fname, 'r')
        ret = fi.read()
        fi.close()
        return ret

    def get_declarations(self):
        #
        #  Should return a string which represents legal C declarations for
        #   inclusion in a header file.
        #

        compiler_command = [self.cfg.compiler]          # Compiler path
        compiler_command.append('-E')                   # Pre-processor only, to stdout
        compiler_command.append('-P')                   # Don't generate line markers in the output
        compiler_command.append('-DGEN_CONFIG_HEADER')  # The C code we're processing uses this to indicate header file generation only
        for f in self.cfg.includes:
            compiler_command.extend(['--include', f])   # If the caller requested include files, pass them to the compiler here
        compiler_command.extend(['-xc', '-'])           # Passing -xc - to the compiler tells it to process stdin, and use C semantics

        c_code = self.get_template_file()

        #
        #  Replace QuRT constants with their values if appropriately marked.  For instance,
        #   all instances of the string {:SIZEOF_TCB} will be replaced by the actual size
        #   of a QuRT TCB.
        #

        for k,v in qurt_consts.__dict__.items():
            if not k.startswith('_'):
                c_code = re.sub(r'\{:%s\}' % k, str(v), c_code)

        #
        #  All remaining instances of the form {:XXX} are now replaced with the string 0.
        #   Those instances would indicate parts of the C file that vary depending on the
        #   XML configuration.  Whether they are integers, pointers, or string literals in
        #   the actual C code, replacing them with 0 should result in a legal C construct
        #   for the parser.
        #

        c_code = re.sub(r'\{:[^}]+\}', '0', c_code)

        #
        #  Start the C compiler running here, feed it the modified C code, and catch
        #   the stdout stream.
        #

        process = subprocess.Popen(compiler_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        h_file, _ = process.communicate(c_code)

        if process.returncode:
            raise Exception('While generating QuRT configuration header\n*** %s\n*** Compiler exited with return code %s' % (' '.join(compiler_command), process.returncode))

        return h_file

    def get_definitions(self):
        #
        #  Should return a string which represents the C code to be compiled
        #   to build cust_config.o.
        #

        c_code = self.get_template_file()

        #
        #  Fill in the template
        #

        for k,v in self.cfg.compiled_dict.items():
            c_code = re.sub(k, v, c_code)

        c_code += getattr(self.cfg,'symtab_code','')
        c_code += getattr(self.cfg,'strtab_code','')

        #
        #  In the past, the template C code used to define section attributes
        #   for its variables.  We now handle this differently.  If any
        #   section attributes are seen, we want to basically no-op them;
        #   this definition has that effect.
        #
        #  For the new mechanism, we use a linker script to place variables
        #   in their correct output sections instead of requiring the C
        #   code to be changed.  Among other advantages, this allows us
        #   to have a single C template, but multiple linker scripts depending
        #   on the target's security and section placement needs.
        #

        if True:
            c_code = '#define section(x) used\n' + c_code

        return c_code
