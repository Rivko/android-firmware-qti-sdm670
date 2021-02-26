'''
   CoreBSP header deprendecy preprocess parser
'''
#===============================================================================
#
# CoreBSP header deprendecy preprocess parser
#
# GENERAL DESCRIPTION
#    Parses preprocess file and generated header dependecy tree
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/hdeptree.py#3 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Author: pwbldsvc $
#  $Change: 7115037 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/01/10   wd      Create
#
#===============================================================================
import os
import re
import sys
from optparse import OptionParser

#------------------------------------------------------------------------------
# Parse command line arguments
#------------------------------------------------------------------------------
def parse_args():
    '''
       parse_args
       
       Parse command line arguments
    '''
    usage = "usage: %prog [options]"
    version = "%prog 1.0"
   
    parser = OptionParser(usage=usage, version=version)
    parser.add_option("-f", "--ppfile", dest="pp_filename",
                      help="Read preprocess data from FILE", metavar="FILE")
    parser.add_option("-o", "--outfile", dest="output_filename",
                      help="Write output to FILE", metavar="FILE")
    parser.add_option("-v", "--verbose",
                     action="store_true", dest="verbose", default=False,
                     help="print status messages to stdout")
          
    (options, args) = parser.parse_args()
   
    if options.pp_filename is None:
        parser.error("-f option must be defined")
        sys.exit(2)
                  
                  
    return (options, args)
   
#------------------------------------------------------------------------------
# Process include lines from preprocess file
#------------------------------------------------------------------------------
def process_inc_lines(options):
    '''
       process_inc_lines

       Process include lines from preprocess file
    '''
    # get info from options
    pp_filename = options.pp_filename

    # process preprocess output find lines with format:
    # RVCT: #line <num> <path>
    # GCC : # <num> <path> other stuff
   
    #import pdb; pdb.set_trace()
    pattern_rvct = \
     re.compile("^#line\s+(\d+)\s+[\"|\<](?P<filename>.*)[\"|\>]$", re.I | re.M)
    pattern_gcc = \
     re.compile("^#\s+(\d+)\s+[\"|\<](?P<filename>.*)[\"|\>].*$", re.I | re.M)

    file_item = open(pp_filename, 'r')
    data = file_item.read()
    file_item.close()
   
    re_match = pattern_rvct.findall(data)
   
    if len(re_match) == 0:
        re_match = pattern_gcc.findall(data)
   
    # find all and return list of (linenum, filename) pairs
    return re_match
   
#------------------------------------------------------------------------------
# Create header dependecy tree
#------------------------------------------------------------------------------
def create_tree(options, inc_lines_data):
    '''
       create_tree

       Create header dependecy tree
    '''
    parent_node = (None, None, None)
    current_node = ("root", [], parent_node)
    root_node = current_node

    name = 0
    child = 1
    parent = 2
   
    # create tree from (linenum, filename) pairs
    # rules are:
    # 1) new file always starts with line number "1'
    # 2) same files can apper in sequence
    # 3) when a none line number "1" is foud it can be the current file or 
    #    previous file.
   
    #import pdb; pdb.set_trace()   
    for linenum, filename in inc_lines_data:
        #print "line %s, file %s" % (linenum,filename)
      
        # cehck for new file
        if linenum == "1":
            # special case for gcc ignore 
            # 1 file_name_a.c
            # 1 "<built-in>"
            # 1 "<command line>"
            # 1 file_name_a.c

            if (filename == "<built-in>") or (filename == "<command line>") or \
               (filename == current_node[name]):
                continue

            # add child
            child_node = (filename, [], current_node)
            current_node[child].append(child_node)
         
            # set parent
            parent_node = current_node

            # set current node to child
            current_node = child_node
         
        # pop to parent node
        elif filename != current_node[name]: 
            current_node = parent_node
            parent_node = current_node[parent]

    return root_node
   
#------------------------------------------------------------------------------
# Display or save output
#------------------------------------------------------------------------------
def save_output(options, root_node):
    '''
       save_output

       Display or save output
    '''
    # get info from options
    output_filename = options.output_filename
    verbose = options.verbose

    #import pdb; pdb.set_trace()     
   
    if output_filename is None:
        file_item = sys.stdout
    else:
        file_item = open(output_filename, 'w')
   
    def process_node(node, indent):
        '''
           process_node
        '''
        name = 0
        child = 1
   
        local_indent = indent+"+-"
        for child_node in node:
            # write to file or stdout
            file_item.write \
                (local_indent + os.path.normpath(child_node[name]) + "\n")
         
            # check verbose flag and adapt
            if verbose and output_filename is not None:
                print local_indent + os.path.normpath(child_node[name])
            
            if len(child_node[child]) > 0:
                #print child_node[name]
                #import pdb; pdb.set_trace()     
                if len(child_node[child]) > 1:
                    child_indent = indent + "| "
                else:
                    child_indent = indent + "  "
                process_node(child_node[child], child_indent)
            
    process_node(root_node[1], "")
         
    if output_filename is not None:
        file_item.close()

#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
def main():
    '''
       main
    '''
   
    # get args from cmd line
    (options, args) = parse_args()
   
    # read file and get include lines
    inc_lines_data = process_inc_lines(options)
   
    #import pdb; pdb.set_trace()     
   
    # process data
    root_node = create_tree(options, inc_lines_data)
   
    # print or save output
    save_output(options, root_node)
   
    #import pdb; pdb.set_trace()     

# call main
main()
