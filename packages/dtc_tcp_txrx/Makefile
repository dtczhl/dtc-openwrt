include $(TOPDIR)/rules.mk

# Huanle Zhang
# www.huanlezhang.com

PKG_NAME:=dtc_tcp_txrx
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	CATEGORY:=DTC_OPENWRT
	TITLE:=TCP sender and receiver
	MAINTAINER:=Huanle Zhang
	URL:=www.huanlezhang.com
	DEFAULT:=y
endef

define Package/$(PKG_NAME)/description
	Customized TCP-based sender and receiver 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
