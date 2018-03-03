#!/bin/bash

# Main controller 

# Huanle Zhang at UC Davis
# www.huanlezhang.com

# --- Configuration Zone ---
KERNEL_PATCHVER=4.9

LINUX_VERSION_4_9=.82

# --- End of configuration zone ---

# get current path of this file
CURRENT_PATH="$(cd "$(dirname "$0")"; pwd -P)"
# get filename of this script
MY_FILENAME="$(basename "$0")"

# colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# --- functions ---

error() {
	# show error info and exit
	
	# change color 
	echo -ne "${RED}"

	echo "***Error in ${CURRENT_PATH}/${MY_FILENAME}:"
	echo "    $@"

	# change back to plain color
	echo -ne "${NC}"
	exit 
}

usage (){
cat <<EOF
$MY_FILENAME 
    --install            install 
    --uninstall          uninstall
    --package-update     update packages
    --package-remove     remove packages 
    --help               show usage() 
EOF
}

argumentProcess (){

	if [ $# -eq 0 ]; then
		# show help info
		usage 
		exit
	fi 
	
	# change color
	echo -ne "${YELLOW}"

	while [[ $# -gt 0 ]]; do
		key="$1"

		case $key in 
		--install) # install 
			# install patches
			echo "install patches"
			cp -pr dtc-openwrt/patches/* .
            
            # x86
            # make sure kernel version is correct
            echo "x86 Makefile in target/linux/x86/"
            # avoid corrupting the original files
            if [ ! -f ./target/linux/x86/Makefile.orig ]; then 
                cp ./target/linux/x86/Makefile ./target/linux/x86/Makefile.orig
                sed -i -e "s/.*KERNEL_PATCHVER.*/#dtc\nKERNEL_PATCHVER=${KERNEL_PATCHVER}/" ./target/linux/x86/Makefile
            fi 
			shift
			;;
		--uninstall) # uninstall
			# uninstall 
			echo "remove patches"
			${CURRENT_PATH}/tools/undo_cp.sh "$(pwd -P)/dtc-openwrt/patches" "$(pwd -P)"

            # x86
            echo "x86 Makefile recover in target/linux/x86"
            cp ./target/linux/x86/Makefile.orig ./target/linux/x86/Makefile
            rm ./target/linux/x86/Makefile.orig
			shift
			;;
		--package-update) # update package
			echo "package update"
            cp -r ${CURRENT_PATH}/packages/* package/feeds/
			shift
			;;
		--package-remove) # remove package
			echo "package remove"
            rm -rf package/feeds/dtc_*
			shift
			;;
		--help)
			echo "help"
			shift
			;;
		*) # unkown argument
			error "Unknown argument $key"
			shift
			;;
		esac 
	done

	# recover color
	echo -ne "${NC}"
}

# --- end of functions ---

# --- main starts here ---

# check caller directory
if [ ! -d "dtc-openwrt" ]; then
	error "Wrong caller location. There is no dtc-openwrt folder under current folder"
fi

# process arguments
argumentProcess $@

# --- end of main ---
