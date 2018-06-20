#!/bin/bash

# format CSV data to more readable structure
#
# Huanle Zhang at UC Davis
# www.huanlezhang.com

myFormat=
myInputFile=
myVerbose=0

usage (){
cat <<EOF
Usage: csv2plain.sh 
        --format format_string (t1,t2,ip,num,raw)
                t1,t2: combine to a timestamp 
                ip: ip numeral to string
                num: convert bytes to number
                raw: keep unchanged
        --input input_file
        --verbose
E.g., csv2plan.sh --format "t1,t2,num" --input file.csv 
EOF
}

argumentProcess (){
    if [ $# -lt 1 ]; then
        usage
        exit 
    fi 

    while [[ $# -gt 0 ]]; do
        key="$1"
        case $key in
        --format) 
            shift
            myFormat="$1"
            shift 
            ;;
        --input)
            shift
            myInputFile="$1"
            shift 
            ;;
        --verbose) 
            myVerbose=1
            shift 
            ;;
        *) # unknown arguments
                echo "*** Error unknown argument"
                echo -e "\t $1"
                exit
            ;;
        esac
    done

    # check arguments
    if [[ -z "$myFormat" || -z "$myInputFile" ]]; then
        echo "*** Error empty argument"
        echo "--format $myFormat"
        echo "--input $myInputFile"
        exit
    fi

    # display arguments
    if [[ $myVerbose -eq 1 ]]; then
        echo "Arguments: "
        echo -e "\t --format $myFormat"
        echo -e "\t --input $myInputFile"
        echo -e "\t --verbose True"
    fi
}

dataProcess (){
    case $myFormat in
    "t1,t2,num")
        
        awk -F "," '
            {
                printf "%d,", (1000000000*$1+$2);
                
                split($3, arr, "-");
                num=(255**3)*arr[1]+(255**2)*arr[2]+(255**1)*arr[3]+(255**0)*arr[4];
                print num 
            }
        ' $myInputFile > ${myInputFile}.plain
        ;;
    "t1,t2,ip,num")
        awk -F "," '
            {
                printf "%d,", (1000000000*$1+$2);

                ip_4 = rshift($3, 0) % 256; ip_3 = rshift($3, 8) % 256; 
                ip_2 = rshift($3, 16) % 256; ip_1 = rshift($3, 24) % 256;
                printf "%d.%d.%d.%d,", ip_1, ip_2, ip_3, ip_4;

                split($4, arr, "-");
                num=(255**3)*arr[1]+(255**2)*arr[2]+(255**1)*arr[3]+(255**0)*arr[4];
                print num 
            }
        ' $myInputFile > ${myInputFile}.plain
        ;;
    "t1,t2,raw,num")
         awk -F "," '
            {
                printf "%d,", (1000000000*$1+$2);
                
                printf "%s,", $3

                split($4, arr, "-");
                num=(255**3)*arr[1]+(255**2)*arr[2]+(255**1)*arr[3]+(255**0)*arr[4];
                print num 
            }
        ' $myInputFile > ${myInputFile}.plain

        ;;
    *) # unknown
        echo "*** Error unknown argument:"
        echo -e "\t $myFormat"
        exit 
        ;;
    esac 
}

cleanup (){
    if [[ $myVerbose -eq 0 ]]; then
        :
    fi
}

# --- main starts here ---
argumentProcess $@
dataProcess
cleanup
