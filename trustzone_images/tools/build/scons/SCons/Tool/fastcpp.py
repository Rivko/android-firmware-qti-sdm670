
"""SCons.Tool.fastcpp

A Tool specialized on speeding up large CPP builds with many files.
It replaces the default list of allowed CPP suffixes and also installs
a very simple CPP scanner.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.

"""

#
# Copyright (c) 2001-7,2010,2011,2012,2013 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

import os
import re

import SCons.Scanner
import SCons.Environment
import SCons.Tool
import SCons.Util

def expand_prefixes(env, builder, prefix=None, suffix=None, src_suffix=None):
    if isinstance(builder, SCons.Util.Proxy):
        # For composite builders get at the wrapped proxy object
        builder = builder.get()
    if prefix is not None:
        builder.prefix = env.subst(prefix)
    if suffix is not None:
        builder.set_suffix(env.subst(suffix))
    if src_suffix is not None:
        builder.set_src_suffix(env.subst(src_suffix))

def generate(env):
    """Add Builders and construction variables to the Environment."""

    env.SetDefault(
        FAST_CPPSUFFIXES = ['.c','.cpp','.h','.inc','.s']
    )

    #
    # Replace default CPP suffix list
    #
    env['CPPSUFFIXES'] = env['FAST_CPPSUFFIXES']
    env['OBJPREFIX'] = env.subst('$OBJPREFIX')
    env['SHOBJPREFIX'] = env.subst('$SHOBJPREFIX')

    #
    # Install simple C/CPP scanner
    #
    cscan = SCons.Scanner.ClassicCPP(name = 'cfile',
                    suffixes = env['FAST_CPPSUFFIXES'],
                    path_variable = 'CPPPATH',
                    regex = r'^\s*(?:include)\s+(<|"?)([^>\s"]+)(?:>|"?)')
    for suffix in env['FAST_CPPSUFFIXES']:
        SCons.Environment._set_SCANNERS(env, suffix, cscan)

    #
    # Switching off implicit dependencies for commands like g++ or ar
    #
    env['IMPLICIT_COMMAND_DEPENDENCIES'] = False

    #
    # Pre-expanding prefixes/suffixes for the most used builders
    #
    static_obj, shared_obj = SCons.Tool.createObjBuilders(env)
    expand_prefixes(env, static_obj, '$OBJPREFIX', '$OBJSUFFIX')
    expand_prefixes(env, shared_obj, '$SHOBJPREFIX', '$SHOBJSUFFIX')

    static_lib = SCons.Tool.createStaticLibBuilder(env)
    expand_prefixes(env, static_lib, '$LIBPREFIX', '$LIBSUFFIX', '$OBJSUFFIX')

    shared_lib = SCons.Tool.createSharedLibBuilder(env)
    expand_prefixes(env, shared_lib, '$SHLIBPREFIX', '$SHLIBSUFFIX', '$SHOBJSUFFIX')

    module = SCons.Tool.createLoadableModuleBuilder(env)
    expand_prefixes(env, module, '$LDMODULEPREFIX', '$LDMODULESUFFIX', '$SHOBJSUFFIX')

    prog = SCons.Tool.createProgBuilder(env)
    expand_prefixes(env, prog, '$PROGPREFIX', '$PROGSUFFIX', '$OBJSUFFIX')

def exists(env):
    return 1
