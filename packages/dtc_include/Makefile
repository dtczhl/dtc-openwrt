include $(TOPDIR)/rules.mk

#
# Huanle Zhang
# www.huanlezhang.com
#

PKG_NAME:=dtc_include
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	CATEGORY:=DTC_OPENWRT
	TITLE:=customized .h and .c files for other packages
	MAINTAINER:=Huanle Zhang
	URL:=www.huanlezhang.com
	DEFAULT:=y
endef

define Package/$(PKG_NAME)/description
	Provide common .h .c files for other packages
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src $(PKG_BUILD_DIR)/

	# empty Makefile 
	echo "target: ;" > $(PKG_BUILD_DIR)/Makefile
endef

define Package/$(PKG_NAME)/install
	# do nothing
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
