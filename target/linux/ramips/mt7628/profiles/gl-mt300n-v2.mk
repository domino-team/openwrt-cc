#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/GL-MT300N-V2
	NAME:=GL-iNet GL-MT300N-V2
	PACKAGES:=\
	kmod-usb-core kmod-usb2 kmod-usb-ohci \
	kmod-ledtrig-usbdev
endef

define Profile/GL-MT300N-V2/Description
 Support for GL-iNet GL-MT300N-V2 routers
endef
$(eval $(call Profile,GL-MT300N-V2))
