#!/bin/bash

# format CSV data to more readable structure
#
# Huanle Zhang at UC Davis
# www.huanlezhang.com

usage (){
cat <<EOF
Usage: csv2plain.sh 
        --format format_string (t1,t2,r)
        --input input_file
E.g., csv2plan.sh --format "t1,t2,r" --input file.csv 
EOF
}

argumentProcess (){
    if [ $# -lt 1 ]; then
        usage
        exit 
    fi 


}

argumentProcess $@
