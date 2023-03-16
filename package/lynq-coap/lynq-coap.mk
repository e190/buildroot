################################################################################
#
# helloworld
#
################################################################################

LYNQ_COAP_VERSION:= 1.0.0
LYNQ_COAP_SITE:= $(CURDIR)/mobiletek/samples/coap
LYNQ_COAP_SITE_METHOD:=local
LYNQ_COAP_INSTALL_TARGET:=YES

define LYNQ_COAP_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_COAP_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/coap_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_COAP_PERMISSIONS
    /usr/bin/coap_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
