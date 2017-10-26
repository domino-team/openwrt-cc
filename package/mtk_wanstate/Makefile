#
#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mtk_wanstate
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/mtk_wanstate
  SECTION:=base
  CATEGORY:=gl-inet
  TITLE:=Change wan port status for MediaTek chip
  DEPENDS:=@TARGET_ramips
endef

define Package/mtk_wanstate/description
This program to fix wan port state for MediaTek chip, it can't
receive netlink event on application layer, which cause the router
can't reacquire ip address when changing cable. 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Package/mtk_wanstate/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtk_wanstate $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/mtk_wanstate.config $(1)/etc/config/mtk_wanstate
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mtk_wanstate.init $(1)/etc/init.d/mtk_wanstate
endef

$(eval $(call BuildPackage,mtk_wanstate))
