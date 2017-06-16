
# Copyright (C) 2015 Onion Corporation
#
# Author: Lazar Demin  <lazar@onion.io>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=nodejs
PKG_VERSION:=0.1
PKG_RELEASE:=1


PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=HEAD
PKG_SOURCE_URL:=https://github.com/OnionIoT/nodejs.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION)-$(PKG_RELEASE).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/libv8
	SECTION:=domino
	CATEGORY:=domino
	SUBMENU:=Libraries
	TITLE:=Library for Google's V8 Javascript Engine Port
	DEPENDS:=+libpthread +libstdcpp $(CXX_DEPENDS)
endef

define Package/libv8/description
Library for Google's V8 Javascript Engine Port
endef

define Package/nodejs
	SECTION:=domino
	CATEGORY:=domino
	SUBMENU:=Languages
	TITLE:=Event-driven I/O server-side JavaScript environment based on V8
	DEPENDS:=+libv8 +libpthread +librt +libstdcpp $(CXX_DEPENDS)
endef


define Package/nodejs/description
Event-driven I/O server-side JavaScript environment based on V8
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include $(1)/usr/lib
	$(CP)   $(PKG_BUILD_DIR)/v8/include/v8.h $(1)/usr/include/
	$(CP)   $(PKG_BUILD_DIR)/v8/lib/libv8.so $(1)/usr/lib/
endef

### installing from precompiled lib and binary from repo
### 	issue with compile on strider (due to long file paths)
#define Build/Configure
#	# running configure
#	$(PKG_BUILD_DIR)/configure \
#		--without-snapshot \
#		--shared-v8 \
#		--shared-v8-includes="$(PKG_BUILD_DIR)/v8/include/" \
#		--shared-v8-libpath="$(PKG_BUILD_DIR)/v8/lib" \
#		--dest-cpu=mips
#endef

#define Build/Compile
#	# running compile
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		CC=mips-openwrt-linux-gcc \
#		CXX=mips-openwrt-linux-g++ \
#		AR=mips-openwrt-linux-ar \
#		RANLIB=mips-openwrt-linux-ranlib \
#		LINK=mips-openwrt-linux-g++ \
#		snapshot=off
#endef

define Build/Configure
	# do nothing!
endef

define Build/Compile
	# do nothing!
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		omega
endef

define Package/libv8/install
	# install the static library object
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)   $(PKG_BUILD_DIR)/v8/lib/libv8.so $(1)/usr/lib/
endef

define Package/nodejs/install
	$(INSTALL_DIR) $(1)/usr/bin
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/out/Release/node $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/deploy/bin/node $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libv8))
$(eval $(call BuildPackage,nodejs))

