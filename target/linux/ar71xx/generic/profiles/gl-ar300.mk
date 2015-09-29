#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/GL-AR300
	NAME:=GL AR300
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/GL-AR300/Description
	Configuration of GL AR300.
endef

$(eval $(call Profile,GL-AR300))
