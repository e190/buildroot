################################################################################
#
# helloworld
#
################################################################################

LYNQ_HTTP_VERSION:= 1.0.0
LYNQ_HTTP_SITE:= $(CURDIR)/mobiletek/samples/http
LYNQ_HTTP_SITE_METHOD:=local
LYNQ_HTTP_INSTALL_TARGET:=YES

define LYNQ_HTTP_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            LYNQ_DIR="$(CURDIR)/mobiletek/" -C $(@D) all
endef

define LYNQ_HTTP_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/http_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_HTTP_PERMISSIONS
    /usr/bin/http_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
