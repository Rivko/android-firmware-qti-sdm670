#===============================================================================
# Script to configure nanopb dependency for SSC build.
#
# GENERAL DESCRIPTION
#    Script to configure nanopb dependecy for SSC builds.
#
# Copyright (c) 2016 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#===============================================================================
import sys, getopt
import os
import platform
import tarfile
import zipfile
import inspect
import shutil

def main(argv):
    if "Windows" in platform.system() or "CYGWIN" in platform.system():
	  in_file = r"nanopb-0.3.6-windows-x86"
    else:
      in_file = r"nanopb-0.3.6-linux-x86"
    path = os.getcwd()
    print path 
    try:
        opts, args = getopt.getopt(argv,"hf:",["ifile="])
    except getopt.GetoptError:
        print 'python config_nanopb_dependency.py -f <nanopb tar/zip file name>'
        print 'Example:python config_nanopb_dependency.py -f nanopb-0.3.6-linux/windows-x86'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 'python config_nanopb_dependency.py -f <nanopb tar/zip file name>'
            print 'Example:python config_nanopb_dependency.py -f nanopb-0.3.6-linux/windows-x86'
            sys.exit()
        elif opt in ("-f", "--ifile"):
            me = inspect.getfile(inspect.currentframe())
            my_root = os.path.dirname(os.path.dirname(me))

            # Create \ssc\tools\nanopb folder
            ssc_tools_path = os.path.join(my_root, "tools")
            ssc_nanopb_path = os.path.join(my_root, "tools", "nanopb")
            if not os.path.exists(ssc_nanopb_path):
                os.mkdir(ssc_nanopb_path);

            # Construct file name for input zip file
	    # Unzip the input file into \ssc\tools folder
            if "Windows" in platform.system() or "CYGWIN" in platform.system():
              in_file = (ssc_tools_path+'/'+arg+'.zip').replace('\\', '/').rstrip('\n')			
              print in_file			
              zip_file = zipfile.ZipFile(in_file, 'r')
              zip_file.extractall(ssc_tools_path)
              src_path = os.path.join(ssc_tools_path, arg, "generator-bin")
              ssc_proto_gen_path = os.path.join(my_root, "tools", "nanopb", "generator-win")			  
            else:
              in_file = (ssc_tools_path+'/'+arg+'.tar.gz').replace('\\', '/').rstrip('\n')			
              print in_file
              tar_file = tarfile.open(in_file, 'r')
              tar_file.extractall(ssc_tools_path)
              src_path = os.path.join(ssc_tools_path, arg, "generator-bin")
              ssc_proto_gen_path = os.path.join(my_root, "tools", "nanopb", "generator")

            # Copy the generator-bin folder from extracted file into \ssc\tools\nanopb\generator-win
            if os.path.isdir(ssc_proto_gen_path):
                shutil.rmtree(ssc_proto_gen_path) 

            shutil.copytree(src_path, ssc_proto_gen_path)

            # Look for .egg files and extract them
            for file in os.listdir(ssc_proto_gen_path):
                if file.endswith(".egg"):
                 print(file)
                 in_file = (ssc_proto_gen_path+'/'+file).replace('\\', '/').rstrip('\n')
                 print(in_file)
                 if os.path.isfile(in_file):
                   if "Windows" in platform.system() or "CYGWIN" in platform.system():
                     zip_file = zipfile.ZipFile(in_file, 'r')
                     zip_file.extractall(ssc_proto_gen_path)
                   else:
                     tar_file = tarfile.open(in_file, 'r')
                     tar_file.extractall(ssc_proto_gen_path)
                 # Do I need to create separate folder for each extract?

            # Copy over *.h files
            src_path = os.path.join(ssc_tools_path, arg)
            dst_path = os.path.join(my_root, "inc", "utils", "nanopb")
            print src_path
            print dst_path
            for file in os.listdir(src_path):
                if file.endswith(".h"):
                    print(file)
                    in_file = (src_path+'/'+file).replace('\\', '/').rstrip('\n')
                    shutil.copy(in_file,dst_path)

            dst_path = os.path.join(my_root, "utils", "nanopb", "src")
            print dst_path
            for file in os.listdir(src_path):
                if file.endswith(".c"):
                    print(file)
                    in_file = (src_path+'/'+file).replace('\\', '/').rstrip('\n')
                    shutil.copy(in_file,dst_path)
            # Copy over nanopb.proto and descriptor.proto files
            src_path = os.path.join(ssc_tools_path, arg, "generator", "proto")
            src_file = (src_path+'/'+'nanopb.proto').replace('\\', '/').rstrip('\n')
            dst_path = os.path.join(my_root, "utils", "nanopb", "pb")
            if not os.path.exists(dst_path):
                os.mkdir(dst_path);
            shutil.copy(src_file,dst_path)
            src_path = os.path.join(ssc_tools_path, arg, "generator", "proto", "google", "protobuf")
            src_file = (src_path+'/'+'descriptor.proto').replace('\\', '/').rstrip('\n')
            dst_path = os.path.join(my_root, "utils", "nanopb", "pb", "google")
            if not os.path.exists(dst_path):
                os.mkdir(dst_path);
            dst_path = os.path.join(my_root, "utils", "nanopb", "pb", "google", "protobuf")
            if not os.path.exists(dst_path):
                os.mkdir(dst_path);
            shutil.copy(src_file,dst_path)

        else:
            print 'Incorrect input'

if __name__ == "__main__":
    main(sys.argv[1:])
    
