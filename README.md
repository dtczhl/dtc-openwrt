# dtc-openwrt
Customized OpenWrt system

### Check and install required tools

Refer to [OpenWrt build system -- Installation](https://wiki.openwrt.org/doc/howto/buildroot.exigence)
    
I copied the command to install the tools for Ubuntu 64bit here

`sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev unzip`
        

## Install
1. Create an empty folder and cd there. E.g, 

      `mkdir dtcOpenWrt`
      
      `cd dtcOpenWrt`
     
2. Download OpenWrt system. To be compatible, better download the version that I forked from OpenWrt/LEDE

    `git clone https://github.com/dtczhl/openwrt-fork.git .`
  
3. Download this repo. Must be placed in the root directory of OpenWrt source

    `git clone https://github.com/dtczhl/dtc-openwrt.git`
    
4. Install my customized system. Run 

    `./dtc-openwrt/main.sh --install`
    
    It will modify kernels, install my packages, etc...
    
5. Build as usual way of building OpenWrt, run

    `./scripts/feeds install -a`
    
    `./scripts/feeds update -a`
    
    `make menuconfig`  My packages are under `DTC` tag
    
    
## Uninstall

1. Go to the root directory of OpenWrt source, run

    `./dtc-openwrt/main.sh --uninstall`
    
    It rolls back to the official version before my modifications.
    
## Package Updating and Removing

1. If you modify my packages, then to update them, run
    
    `./dtc-openwrt/main.sh --package-update`
    
2. To remove my packages, run

    `./dtc-openwrt/main.sh --package-remove`
