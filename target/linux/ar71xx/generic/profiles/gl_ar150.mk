#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/GL_AR150
	NAME:=GL_ar150
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/GL_AR150/Description
	Configuration of GL_ar150.
endef

$(eval $(call Profile,GL_AR150))
