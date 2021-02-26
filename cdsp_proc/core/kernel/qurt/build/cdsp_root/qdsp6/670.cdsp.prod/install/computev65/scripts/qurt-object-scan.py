#=============================================================================
#
#                                    qurt-object-scan.py
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
#!/usr/bin/env python
#
#  qurt-report.py takes a list of object files and scans them for
#   various things that could cause problems.
#
#  The initial implementation of this is designed to scan for the
#   following potential issues, specifically for the QuRT microkernel,
#   but can be used for other purposes:
#
#  1)  References which are not satisfied within the list of object
#      files.  Indicates external dependencies.
#  2)  Global symbols which are not referenced outside their own
#      module.  Indicates potential modularity problems.
#

def do_scan(outfile, tool_prefix, args):
    import subprocess

    for fname in args:
        outfile.write('Scan %s\n' % fname)
    outfile.write('\n')

    nm_process = subprocess.Popen([tool_prefix + 'hexagon-nm'] + args, stdout=subprocess.PIPE)

    nm_output = nm_process.communicate()[0]

    fname = '/dev/null'

    common_defs = dict()
    strong_exports = dict()
    strong_imports = dict()
    weak_exports = dict()
    weak_imports = dict()

    for s in nm_output.splitlines():
        if s.endswith(':'):
            # Ends with a colon.  Assume it's changing the filename
            fname = s[:-1]
            continue
        x = s.split(None,1)
        if len(x) == 0:
            # Blank or only white space
            continue
        if s.startswith(x[0]):
            # Line starts with a token, assume it's a hexadecimal number indicating an export
            value = x[0]
            x = x[1].split(None, 1)
        else:
            # Line does not start with a token, assume it's an import
            value = None
        if len(x) != 2:
            raise Exception('Cannot parse line from hexagon-nm: %s' % s)
        nmtype, symbol = x[0], x[1]

        # Find out which type of symbol this is
        if value != None and (nmtype == 'a' or
                              nmtype == 'b' or
                              nmtype == 'd' or
                              nmtype == 'r' or
                              nmtype == 't'):
            # Local symbol; ignore it
            continue
        if value != None and (nmtype == 'A' or
                              nmtype == 'B' or
                              nmtype == 'D' or
                              nmtype == 'R' or
                              nmtype == 'T'):
            strong_exports.setdefault(symbol, [])
            strong_exports[symbol].append(fname)
            continue
        if value != None and nmtype == 'C':
            common_defs.setdefault(symbol, [])
            common_defs[symbol].append(fname)
            continue
        if value == None and nmtype == 'U':
            strong_imports.setdefault(symbol, [])
            strong_imports[symbol].append(fname)
            continue
        if value != None and (nmtype == 'V' or
                              nmtype == 'W'):
            weak_exports.setdefault(symbol, [])
            weak_exports[symbol].append(fname)
            continue
        if value == None and (nmtype == 'v' or
                              nmtype == 'w'):
            weak_imports.setdefault(symbol, [])
            weak_imports[symbol].append(fname)
            continue

        # Did not recognize the symbol; put a warning into the output file
        outfile.write('*** Could not parse nm output: <%s>\n' % s)

    # Generate the list of external dependencies which are not satisfied
    #  from within the symbols we have
    for symbol, modules in sorted(strong_imports.items()):
        if not symbol in strong_exports:
            if not symbol in weak_exports:
                if not symbol in common_defs:
                    outfile.write('*** Symbol referenced in these objects, but not defined here: %s\n' % symbol)

    # Generate the list of symbols we export which are not referenced
    #  outside their own object file
    for symbol, modules in sorted(strong_exports.items()):
        if not symbol in strong_imports:
            if not symbol in weak_imports:
                if not symbol in common_defs:
                    outfile.write('*** Symbol exported in these objects, but not otherwise used here: %s\n' % symbol)

    return 0

def qurt_object_scan(args):
    import os
    import re	
    from optparse import OptionParser

    parser = OptionParser('%prog input_objects... [options]')
    parser.add_option('-o', '--output', dest='output_file', action='store', help='Output file name')
    parser.add_option('-T', '--tools_dir', dest='tools_dir', action='store', help='Tools directory')
    opts, args = parser.parse_args(args[1:])

    if not opts.tools_dir:
        tool_prefix = ''                                 # Hope that hexagon binutils are on the path
    else:
	    hex_tools_ver = os.path.split(opts.tools_dir)[1]
	    toolsver_match = re.search('^(\d+)\.(\d+).*', hex_tools_ver)
	    if toolsver_match.group(1) > '6':
		   tool_prefix = '%s/Tools/bin/' % opts.tools_dir     # Where to find hexagon binutils
	    else:
		   tool_prefix = '%s/gnu/bin/' % opts.tools_dir     # Where to find hexagon binutils
    if not opts.output_file:
        raise Exception('%s requires an output file' % args[0])

    if not args:
        raise Exception('%s requires at least one input file' % args[0])

    outfile = open(opts.output_file, 'w')
    ret = do_scan(outfile, tool_prefix, args)
    outfile.close()

    return ret

if __name__ == '__main__':
    import sys
    sys.exit(qurt_object_scan(sys.argv))

