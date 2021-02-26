#! /usr/bin/env python
### Replace CRLF with LF in files with extensions given in the below list under the folder given as argument to the script.
### Print names of changed files.
### This script expects dos2unix being installed on the host machine
       
import os, sys
import subprocess

extensions = ['.cmd']
folder = str(sys.argv[1])
print folder
for root, dirs, files in os.walk(folder):
    for file in files:
        if file.endswith(tuple(extensions)):
             filepath = os.path.join(root, file)
             convert_eol_cmd = 'unix2dos '+filepath
             print convert_eol_cmd
             proc = subprocess.Popen(convert_eol_cmd, shell=True)
             (out, err) = proc.communicate()
             
