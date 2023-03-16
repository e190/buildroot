################################################################################
#
# helloworld
#
################################################################################

LYNQ_POLARSSL_VERSION:= 1.0.0
LYNQ_POLARSSL_SITE:= $(CURDIR)/mobiletek/samples/polarssl
LYNQ_POLARSSL_SITE_METHOD:=local
LYNQ_POLARSSL_INSTALL_TARGET:=YES

define LYNQ_POLARSSL_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_POLARSSL_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/polarssl_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_POLARSSL_PERMISSIONS
    /usr/bin/polarssl_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
