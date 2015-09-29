#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/DOMINO
	NAME:=Domino wifi for things
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/DOMINO/Description
	Configuration of Domino, Wifi for everything.
endef

$(eval $(call Profile,DOMINO))
