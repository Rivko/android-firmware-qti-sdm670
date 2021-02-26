import sys
import os
import ConfigParser
import fnmatch

from subprocess import call
from elftools.elf import elffile, constants
from tabulate import tabulate
from optparse import OptionParser


class ElfParser(object):

    def __init__(self, elf_file):
        self.fp = open(elf_file, 'rb')
        self.elf = elffile.ELFFile(self.fp)

    def _section_size(self, flags, sh_type):
        size = 0
        for section in self.elf.iter_sections():
            if (section.header['sh_flags'] & flags == flags) and (section.header['sh_type'] == sh_type):
                size += section.header['sh_size']

        return size

    def get_code_size(self):
        #CODE section is AX & progbits
        return self._section_size((constants.SH_FLAGS.SHF_ALLOC | constants.SH_FLAGS.SHF_EXECINSTR), 'SHT_PROGBITS')

    def get_rwdata_size(self):
        #RW section is AW & progbits
        return self._section_size((constants.SH_FLAGS.SHF_ALLOC | constants.SH_FLAGS.SHF_WRITE), 'SHT_PROGBITS')

    def get_zidata_size(self):
        #ZI section is AW & nobits
        return self._section_size((constants.SH_FLAGS.SHF_ALLOC | constants.SH_FLAGS.SHF_WRITE), 'SHT_NOBITS')

    def get_rodata_size(self):
        # RO section is A & not(X) & not(W) & progbits
        size = 0
        for section in self.elf.iter_sections():
            if (section.header['sh_flags'] & constants.SH_FLAGS.SHF_ALLOC) and \
                    not(section.header['sh_flags'] & constants.SH_FLAGS.SHF_EXECINSTR) and \
                    not(section.header['sh_flags'] & constants.SH_FLAGS.SHF_WRITE) and \
                    (section.header['sh_type'] == 'SHT_PROGBITS'):
                size += section.header['sh_size']
        return size

    def close_elf(self):
        self.fp.close()


def get_report(o_filelist, lib_filelist):

    report_text = ""

    # object file elements
    o_table = []
    o_errors = []
    o_totals = []

    # library file elements
    lib_table = []
    lib_errors = []
    lib_totals = []

    # object and library size counters
    o_total_code_size = o_total_rodata_size = o_total_rwdata_size = o_total_zidata_size = 0
    lib_total_code_size = lib_total_rodata_size = lib_total_rwdata_size = lib_total_zidata_size = 0
    totals = []

    # Process .o file table information
    for filename in o_filelist:
        try:
            r = ElfParser(filename)
            o_table.append([r.get_code_size(), r.get_rodata_size(),  r.get_rwdata_size(), r.get_zidata_size(), os.path.basename(filename) , filename])
            o_total_code_size += r.get_code_size()
            o_total_rodata_size += r.get_rodata_size()
            o_total_rwdata_size += r.get_rwdata_size()
            o_total_zidata_size += r.get_zidata_size()

            r.close_elf()

        except Exception, e:
            o_errors.append(str(os.path.basename(filename)) + ' - ' + str(e))

    # Process .lib file table information
    for filename in lib_filelist:
        try:
            ofiles_from_lib = extract_from_lib(filename)
            lib_code_size = lib_rodata_size = lib_rwdata_size = lib_zidata_size = 0

            for ofile in ofiles_from_lib:
                r = ElfParser(ofile)

                lib_code_size += r.get_code_size()
                lib_total_code_size += r.get_code_size()

                lib_rodata_size += r.get_rodata_size()
                lib_total_rodata_size += r.get_rodata_size()

                lib_rwdata_size += r.get_rwdata_size()
                lib_total_rwdata_size += r.get_rwdata_size()

                lib_zidata_size += r.get_zidata_size()
                lib_total_zidata_size += r.get_zidata_size()

                # Delete extracted .o file from directory
                r.close_elf()
                os.remove(ofile)

            lib_table.append([lib_code_size, lib_rodata_size,  lib_rwdata_size, lib_zidata_size, os.path.basename(filename) ,filename])

        except Exception, e:
            lib_errors.append(str(os.path.basename(filename)) + ' - ' + str(e))

    o_headers = ['Code', 'RO Data', 'RW Data', 'ZI Data', 'Object Name']
    lib_headers = ['Code', 'RO Data', 'RW Data', 'ZI Data', 'Library Name']
    totals_headers = ['Code', 'RO Data', 'RW Data', 'ZI Data', 'Totals Category']

    o_totals.append([o_total_code_size, o_total_rodata_size, o_total_rwdata_size, o_total_zidata_size, 'Object Totals'])
    lib_totals.append([lib_total_code_size, lib_total_rodata_size, lib_total_rwdata_size, lib_total_zidata_size, 'Library Totals'])

    totals.append([o_total_code_size + lib_total_code_size, o_total_rodata_size + lib_total_rodata_size,
                   o_total_rwdata_size + lib_total_rwdata_size, o_total_zidata_size + lib_total_zidata_size, 'Grand Total'])

    # Object file-table
    report_text = report_text + '\n' + str(tabulate(o_table, o_headers, subtotal=o_totals)) + '\n'
    for error in o_errors:
        report_text = report_text + str(error) + '\n'

    # Library file-table
    report_text = report_text + '\n\n' + str(tabulate(lib_table, lib_headers, subtotal=lib_totals)) + '\n'
    for error in lib_errors:
        report_text = report_text + str(error) + '\n'

    # Totals table
    report_text = report_text + '\n\n' + str(tabulate(totals, totals_headers)) + '\n'

    # Print subtotals below totals table
    report_text = report_text + 'Total RO Size (Code + RO Data)\t\t\t' + \
        str(o_total_code_size + lib_total_code_size + o_total_rodata_size + lib_total_rodata_size) + '\n'
    report_text = report_text+ 'Total RW Size (RW Data + ZI Data)\t\t' + \
        str(o_total_rwdata_size + lib_total_rwdata_size + o_total_zidata_size + lib_total_zidata_size) + '\n'
    report_text = report_text + 'Total ROM Size (Code + RO Data + RW Data)\t' + \
        str(o_total_code_size + lib_total_code_size + o_total_rodata_size + lib_total_rodata_size +
        o_total_rwdata_size + lib_total_rwdata_size) + '\n\n'
    return str(report_text)

def extract_from_lib(lib_filename):

    config = ConfigParser.RawConfigParser()
    config.read('report_generator.cfg')

    ofiles = []

    # Use environment variable if it exists
    if "LLVMBIN" in os.environ:
        tool_path = os.path.join(os.environ["LLVMBIN"], 'llvm-ar')
    # Otherwise use config file
    else:
        tool_path = config.get('tool_path', 'ar_tool_path')

    try:
        call([tool_path, 'x', lib_filename])
        for ofile in os.listdir("."):
            if ofile.endswith(".o"):
                ofiles.append(os.path.normpath(os.path.abspath(ofile)))
    except Exception, e:
        raise RuntimeError("Failed to execute command: " + tool_path + ' x ' + lib_filename)

    return ofiles


def create_report_for_mapfile(mapfile):

    try:
        fp = open(mapfile, 'r')
    except IOError, e:
        raise RuntimeError(str(mapfile) + " - Cannot open supplied map file.")

    o_filelist = []
    lib_filelist = []

    for line in fp:
        line = line.strip(' \t\r\n')
        if line[:4] == 'LOAD':
            data = line.split(" ")
            if '(' in data[1] and ')' in data[1] :
                libname = data[1].split('(')[0]
                lib_filelist.append(libname)
            else :
                obj = data[1].split('[')[0]
                o_filelist.append(obj)

    print '\n\n***** Processing Report: ' + mapfile + '.linkreport'
    report_text = get_report(sorted(set(o_filelist)), sorted(set(lib_filelist)))

    try:
        fp.close()
    except IOError, e:
        raise RuntimeError("Failed to close map file.")

    return report_text

if __name__ == '__main__':

    usage = "\nusage: " + sys.argv[0] + " mapfile.map\n       " + sys.argv[0] + " metabuild_dir/\n"

    parser = OptionParser(usage=usage)
    (options, args) = parser.parse_args()

    if len(args) != 1:
        print usage
        sys.exit(1)

    mapfiles = []

    if os.path.isdir(args[0]):

        for root, dirnames, filenames in os.walk(args[0]):
            for filename in fnmatch.filter(filenames, '*.map'):
                mapfiles.append(os.path.abspath(os.path.join(root, filename)))

        for mapfile in mapfiles:

            report_filename = mapfile + '.linkreport'

            # Delete existing .linkreport before creating new .linkreport file
            try:
                os.remove(report_filename)
            except OSError:
                pass

            # Create .linkreport file
            try:
                fp = open(report_filename, "a")
            except Exception, e:
                print "Failed to create file! - " + str(e)

            reportfile_header = '**************************************\n' + \
                                '**************************************\n\n' + \
                                'REPORT: ' + report_filename + '\n\n' + \
                                '**************************************\n' + \
                                '**************************************\n'
            fp.write(reportfile_header)
            mapfile_report = create_report_for_mapfile(mapfile)
            fp.write(mapfile_report)
            fp.close()
    else:
        print create_report_for_mapfile(args[0])