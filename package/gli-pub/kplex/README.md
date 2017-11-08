kplex for OpenWrt
=================

These files are for using the OpenWrt build system to build [kplex](http://www.stripydog.com/kplex/) for OpenWrt.

kplex is a program for combining and routing NMEA-0183 data to and from multiple sources and destinations.
By installing it on a USB-enabled router, it is possible to broadcast NMEA data over a local ethernet and/or wifi network, making the data available to computers, phones, and tablets connected to the network.

[For more information about kplex see http://www.stripydog.com/kplex/](http://www.stripydog.com/kplex/).

## Background

To run kplex on your router, it must be compiled for your router's architecture.
Rather than compiling software on the router directly, as we would on most computers, it is normal to use a process called cross-compiling whereby the software is compiled on a more powerful machine, but with your router's architecture as the target.

Fortunately, OpenWrt makes cross-compiling software very easy, using the [OpenWrt SDK](http://wiki.openwrt.org/doc/howto/obtain.firmware.sdk) which is pre-compiled for every supported architecture, saving you from having to build the whole system from scratch using the [OpenWrt Buildroot](http://wiki.openwrt.org/doc/howto/buildroot.exigence).

You can find the SDK for your router's architecture from [the OpenWrt downloads page](http://downloads.openwrt.org/). The SDK download you need is found in the same directory as the firmware for your router.
For example, the SDK for the common ar71xx architecture is found [here](http://downloads.openwrt.org/barrier_breaker/14.07/ar71xx/generic/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.8-linaro_uClibc-0.9.33.2.tar.bz2.).


## Building

Download and unpack the SDK.

In the `package` subdirectory of the SDK, create a directory called `kplex`.
Place the contents of this repository in the new `package/kplex` directory.

On the command line, `cd` to the SDK directory and execute the command

    $ make package/kplex/compile

This will download the source code for kplex, cross-compile it for your router, and package it in an IPKG installer package.

The package will be located in the `bin` subdirectory of the SDK. For my architecture the package is at `bin/ar71xx/packages/base/kplex_1.1-1_ar71xx.ipk`.


## Installing

Copy the package to your router: (change the filename and the IP address of your router as appropriate)

    $ scp bin/ar71xx/packages/base/kplex_1.1-1_ar71xx.ipk root@192.168.1.1:

SSH into your router and install the package using OPKG.
You have to run `opkg update` first to update the package lists, so that OPKG will know where to download the dependency `libpthreads`.

    $ ssh root@192.168.1.1
    $ opkg update
    $ opkg install kplex_1.1-1_ar71xx.ipk

The commands above should result in `libpthreads` being downloaded and installed, and then kplex being installed.


## Running at Startup

If you want to run kplex automatically at startup, run the following command:

    $ /etc/init.d/kplex enable

Likewise, to disable it, run:

    $ /etc/init.d/kplex disable

If you have the LuCI web interface installed on your router, you can also enable and disable kplex from the `System > Startup` page.


## Configuration

The config file for kplex is located at `/etc/kplex.conf`.
You must edit this file to tell kplex what you want it to do.

[For information on the config file syntax see the kplex website](http://www.stripydog.com/kplex/configuration.html).
