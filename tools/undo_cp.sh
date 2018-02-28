#!/usr/bin/env python3

# remove files according to the source directory structure

# Huanle Zhang at UC Davis
# www.huanlezhang.com

# Note:
#	source_directory and target_directory must be in same format
# Tested: without / after the directory name 
#	dir dir

import os
import sys

def usage():
    print ("Required arguments")
    print ("    1 - source_directory")
    print ("    2 - target_directory")


# --- main starts here ---

if len(sys.argv) != 3:
    print ('***Error')
    print ('    Wrong number of arguments')
    usage ()
    exit ()

source_dir = sys.argv[1]
target_dir = sys.argv[2]

for root, dirs, files in os.walk(source_dir):
    for name in files:
        try:
            dirPrefix = root[len(source_dir):]
            os.remove(target_dir+dirPrefix+"/"+name)
        except FileNotFoundError:
            print ('***Error')
            print ('    FileNotFoundError')
            exit ()

