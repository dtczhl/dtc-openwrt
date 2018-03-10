include $(TOPDIR)/rules.mk

# Huanle Zhang at UC Davis
# www.huanlezhang.com

PKG_NAME:=dtc_udp_redundant_txrx
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	CATEGORY:=DTC_OPENWRT
	TITLE:=UDP sender and receiver, redundant transmission
	MAINTAINER:=Huanle Zhang
	URL:=www.huanlezhang.com
	DEPENDS:=+dtc_include
	DEFAULT:=y
endef

define Package/$(PKG_NAME)/description
	Customized UDP server and client, implementing
	redudant transmission as in MPTCP
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/

	# cp dtc_include files
	$(CP) ../dtc_include/src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
