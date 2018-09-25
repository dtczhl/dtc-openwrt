# dtc-openwrt

Customized OpenWrt OS for research and fun

Huanle Zhang at UC Davis  
[www.huanlezhang.com](www.huanlezhang.com)


## About branch 4.14.63

1. Linux kernel: **4.14.63**
2. [MPTCP](http://multipath-tcp.org/pmwiki.php?n=Main.Release94): **v0.94**


## Install

1. Install required tools. Refer to [OpenWrt build system -- Installation](https://wiki.openwrt.org/doc/howto/buildroot.exigence). For Ubuntu 64bit:

  ```
  sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev unzip
  ```

2. Install OpenWrt

  ```
  git clone https://github.com/openwrt/openwrt.git
  git checkout cad9519ebaf88cb4f3669ca1a04f04eb5d096311
  ```
3. Go to folder (i.e., root directory of OpenWrt), and update feeds

  ```
  ./scripts/feeds update -a
  ./scripts/feeds install -a
  ```

4. Download this repo. Must be placed in the root directory of OpenWrt

  ```
  git clone -b 4.14.63 https://github.com/dtczhl/dtc-openwrt.git 
  ```
