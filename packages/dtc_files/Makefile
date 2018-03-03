include $(TOPDIR)/rules.mk

#
# Huanle Zhang
# www.huanlezhang.com
#

PKG_NAME:=dtc_files
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	CATEGORY:=DTC_OPENWRT
	TITLE:=Base files for DTC_OPENWRT system
	MAINTAINER:=Huanle Zhang
	URL:=www.huanlezhang.com
	DEFAULT:=y
endef

define Package/$(PKG_NAME)/description
	Provide similar functionality as package/base-files
	Copy files to the target OS
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/files
	$(CP) ./files/* $(PKG_BUILD_DIR)/files/

	# empty Makefile 
	echo "target: ;" > $(PKG_BUILD_DIR)/Makefile

endef

define Package/$(PKG_NAME)/install
	$(CP) ./files/* $(1)/	
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
