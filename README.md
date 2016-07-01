This is the buildsystem for OpenWrt CC1505 with updated GLI patches,
including GL-AR150, GL-AR300, GL-Domino, GL-MT300N, GL-MT300A, GL-MT750

Patches has been applied. Just use "make menuconfig" to configure your appreciated
configuration for the toolchain and firmware.

To build your own firmware you need to have access to a Linux, BSD or MacOSX system
(case-sensitive filesystem required). Cygwin will not be supported because of
the lack of case sensitiveness in the file system.


#Use in Ubuntu

```
$ sudo apt-get update
$ sudo apt-get install subversion build-essential git-core libncurses5-dev zlib1g-dev gawk flex quilt libssl-dev xsltproc libxml-parser-perl mercurial bzr ecj cvs unzip git wget
$ git clone https://github.com/domino-team/openwrt-cc.git
$ cd openwrt-cc
$ ./scripts/feeds update -a
$ ./scripts/feeds install -a
$ make menuconfig
  ..choose your compile target and packages
$ make
```

It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications.

You need to have a fast Internet connection otherwise it will takes very long to download the resources, al least several hundreds of MB. If you have all the source already, just put them in your `openwrt-cc/dl` folder and you save time to download resource.

It takes 30 minutes to 1 hours to compile depends on your computer. 
