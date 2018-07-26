# dtc-openwrt
Customized OpenWrt system

Huanle Zhang at UC Davis

[www.huanlezhang.com](http://www.huanlezhang.com)

### Check and install required tools

Refer to [OpenWrt build system -- Installation](https://wiki.openwrt.org/doc/howto/buildroot.exigence)
    
I copied the command to install the tools for Ubuntu 64bit here

`sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev unzip`
        

## Install
1. Create an empty folder and cd there. E.g, 

      `mkdir dtcOpenWrt`
      
      `cd dtcOpenWrt`
     
2. Download OpenWrt system. 

    `git clone https://github.com/openwrt/openwrt.git .`
    `git checkout  d8655868ca04d12ad5ed36e26fc63d61b076e33d`

3. Update feeds

    ```shell
    ./scripts/feeds update -a
    ./scripts/feeds install -a
    ```

3. Download this repo. Must be placed in the root directory of OpenWrt source

    `git clone https://github.com/dtczhl/dtc-openwrt.git`
    
4. Install my system configuration. Run 

    `./dtc-openwrt/main.sh --install`
    
    It will modify kernels, install my packages, etc...
    
5. Install my packages. Run

    `./dtc-openwrt/main.sh --package-update`
    
    My packages are installed in package/feeds/	
    
7. Build as usual way of building OpenWrt, run

    `make menuconfig` 

     * My packages are under `DTC` tag
     * Configure and then run
    		`make V=s`

## Uninstall

1. Go to the root directory of OpenWrt source, run

    `./dtc-openwrt/main.sh --uninstall`
    
    It rolls back to the official version before my system configuration.

 2. Remove my packages, run

	`./dtc-openwrt/main.sh --package-remove`
    
## Package Updating and Removing

1. If you modify my packages, then to update them, run
  
    `./dtc-openwrt/main.sh --package-update`
    
2. To remove my packages, run

    `./dtc-openwrt/main.sh --package-remove`
