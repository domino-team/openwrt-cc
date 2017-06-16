#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=domino-iis
PKG_REFV:=1
PKG_RELEASE:=1
PKG_VERSION:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
include $(INCLUDE_DIR)/package.mk

define KernelPackage/domino-iis
  SECTION:=base
  CATEGORY:=domino
  TITLE:=IIS module for Domino Board
  FILES:= $(PKG_BUILD_DIR)/glzt_i2s.ko
  #AUTOLOAD:=$(call AutoLoad,60,glzt_i2s)
  DEPENDS:=+kmod-sound-core
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
	KSRC="$(LINUX_DIR)" \
	KOBJ="$(LINUX_DIR)" \
	ENV_DIR=$(PKG_BUILD_DIR)/ \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCHIVER="$(TARGET_CROSS)ar" \
	COMPILER="$(TARGET_CC)" \
	LINKER="$(TARGET_CROSS)ld" \
	ARCH="$(LINUX_KARCH)"
endef

MAKE_FLAGS += CFLAGS="$(TARGET_CFLAGS) -Wall"

define KernelPackage/domino-iis/install

endef

$(eval $(call KernelPackage,domino-iis))
