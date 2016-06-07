#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/GL-MIFI
	NAME:=GL-MIFI
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/GL-MIFI/Description
	Configuration of GL-MIFI.
endef

$(eval $(call Profile,GL-MIFI))
