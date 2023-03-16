################################################################################
#
# helloworld
#
################################################################################

LYNQ_NET_CONTROL_VERSION:= 1.0.0
LYNQ_NET_CONTROL_SITE:= $(CURDIR)/mobiletek/samples/net_control
LYNQ_NET_CONTROL_SITE_METHOD:=local
LYNQ_NET_CONTROL_INSTALL_TARGET:=YES

define LYNQ_NET_CONTROL_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_NET_CONTROL_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/net_control $(TARGET_DIR)/usr/bin
endef

define LYNQ_NET_CONTROL_PERMISSIONS
    /usr/bin/net_control f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
