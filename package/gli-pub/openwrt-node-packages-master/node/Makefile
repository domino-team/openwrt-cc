#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

##################################################
# Important - define the currect target CPU      #
# ---------------------------------------------- #
# CPU Name: Atheros 9531 650Mhz CPU              #
# CPU Model: MIPS 24Kc V7.4 (mips32)             #
# Machine: AR300M Router                         #
##################################################
#                                                #
export CONFIG_ARCH:=mips
export CONFIG_TARGET_ARCH_PACKAGES:=ar71xx
#                                                #
##################################################
# FPU: not implemented -> FPU kernel emulation   #
##################################################
# on CPU's without FPU we have to enable the     #
# kernel FPU emulation in OpenWrt Kernel or      #
# we get "illegal instruction" on node start     #
export CONFIG_SOFT_FLOAT:=y
#                                                #
##################################################
# Set the correct host architecture to support   #
# host compiler generating host binaries         #
##################################################
#                                                #
export CONFIG_HOST_ARCH:=x64
#                                                #
# to generate the host binaries successfully we  #
# have to install the gcc/g++ multilibs and set  #
# the enviroment vars CXX_host,CC_host,LINK_host #
# sudo apt-get install gcc-multilib g++-multilib #
#                                                #
export CXX_host="g++"
export CC_host="gcc"
export LINK_host="g++"
#                                                #
##################################################

include $(TOPDIR)/rules.mk

PKG_NAME:=node
PKG_VERSION:=v6.9.1
PKG_RELEASE:=1

PKG_SOURCE:=node-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://nodejs.org/dist/${PKG_VERSION}
PKG_MD5SUM:=19f6d668194f37037ecfd585bea9a61f

HOST_BUILD_DEPENDS:=python/host
PKG_BUILD_DEPENDS:=python/host
PKG_INSTALL:=0
PKG_USE_MIPS16:=0

HOST_BUILD_PARALLEL:=0
PKG_BUILD_PARALLEL:=0

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/node
  SECTION:=lang
  CATEGORY:=Languages
  SUBMENU:=Node.js
  TITLE:=Node.js is a platform built on Chrome's JavaScript runtime
  URL:=http://nodejs.org/
  DEPENDS:=+libstdcpp +libopenssl +zlib +USE_UCLIBC:libpthread +USE_UCLIBC:librt
endef

define Package/node/description
  Node.js® is a JavaScript runtime built on Chrome's V8 JavaScript engine. Node.js uses
  an event-driven, non-blocking I/O model that makes it lightweight and efficient. Node.js'
   package ecosystem, npm, is the largest ecosystem of open source libraries in the world.
endef

define Package/node/config
	menu "> Node Build Options"

	config NODEJS_ICU_FULL
		bool "enable full i18n (full-icu: includes all locales, full API)"
		default n

	config NODEJS_ICU_SMALL
		bool "enable small i18n (small-icu: only English, but full API)"
		default n

	config NODEJS_HOST_CPU_32BIT
		bool "Host CPU: 32-bit [x86]  (unselected: 64-bit [x64]) "
		default n

	endmenu
endef

NODEJS_CPU:=$(subst powerpc,ppc,$(subst aarch64,arm64,$(subst x86_64,x64,$(subst i386,ia32,$(ARCH)))))

MAKE_VARS += \
	DESTCPU=$(NODEJS_CPU)

CONFIGURE_ARGS= \
	--dest-cpu=$(NODEJS_CPU) \
	--dest-os=linux \
	--cross-compiling \
	--without-snapshot \
        --shared-zlib \
	--shared-openssl \
	--prefix=/usr


ifeq ($(CONFIG_NODEJS_HOST_CPU_32BIT),y)
CONFIG_HOST_ARCH:=x86
endif
CONFIGURE_ARGS+= \
	--host-cpu=$(CONFIG_HOST_ARCH)


ifeq ($(CONFIG_NODEJS_ICU_FULL),y)
CONFIGURE_ARGS+= \
	--with-intl=full-icu --download=all
else
ifeq ($(CONFIG_NODEJS_ICU_SMALL),y)
CONFIGURE_ARGS+= \
	--with-intl=small-icu
else
CONFIGURE_ARGS+= \
	--without-intl
endif
endif


ifeq ($(findstring mips,$(NODEJS_CPU)),mips)
ifeq ($(CONFIG_SOFT_FLOAT),y)
CONFIGURE_ARGS+= \
	--with-mips-float-abi=soft
endif
endif

ifeq ($(findstring arm,$(NODEJS_CPU)),arm)
ifeq ($(CONFIG_SOFT_FLOAT),y)
CONFIGURE_ARGS+= \
	--with-arm-float-abi=softfp
endif
endif

#logging:=$(shell v='$(CONFIGURE_ARGS)'; echo $$v > TARGET_CONFIGURE_ARGS.LOG)

HOST_CONFIGURE_VARS:=
HOST_CONFIGURE_ARGS:= \
	--dest-os=linux \
	--without-snapshot \
	--shared-zlib \
	--prefix=$(HOST_BUILD_PREFIX)

HOST_CONFIGURE_CMD:=python ./configure

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
endef

define Package/node/install
	mkdir -p $(1)/usr/bin $(1)/usr/lib/node_modules/npm/{bin,lib,node_modules}
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/{node,npm} $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/node_modules/npm/{package.json,LICENSE,cli.js} $(1)/usr/lib/node_modules/npm
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/node_modules/npm/bin/npm-cli.js $(1)/usr/lib/node_modules/npm/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/node_modules/npm/lib/* $(1)/usr/lib/node_modules/npm/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/node_modules/npm/node_modules/* $(1)/usr/lib/node_modules/npm/node_modules/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,node))

