#
# Copyright (C) 2009-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/GLAR300MNAND
	NAME:=GL-AR300M NAND
	PACKAGES:= kmod-usb-core kmod-usb2 mod-usb-storage \
		kmod-ledtrig-usbdev kmod-ledtrig-gpio
endef

define Profile/GLAR300MNAND/description
	Package set optimized for the GL-AR300M with nand flash.
endef
$(eval $(call Profile,GLAR300MNAND))
